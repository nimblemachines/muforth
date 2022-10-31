/**************************************************
 *
 * System initialization.
 *
 * Copyright 2019 IAR Systems AB.
 *
 **************************************************/

#include "iarMacros.m"
#include "iarCfi.m"

        MODULE  ?cstartup

        PUBLIC  __iar_program_start
        PUBLIC  __iar_program_start_metal

        SECTION CSTACK:DATA:NOROOT(2)

        // --------------------------------------------------

        SECTION `.cstartup`:CODE:NOROOT(2)
        CODE

        CfiCom  ra,1,0
        CfiCom  ra,1,1
        CfiCom  ra,1,2
        CfiCom  ra,1,3

__iar_program_start:
        REQUIRE ?cstart_init_sp

// ----------
//Init gp (required in by the linker config file, if applicable)
        SECTION `.cstartup`:CODE:NOROOT(1)
        CfiBlk 1,__iar_program_start
        CALL_GRAPH_ROOT __iar_program_start, "Reset"
        CODE
        PUBLIC __iar_cstart_init_gp
__iar_cstart_init_gp:
        cfi ?RET Undefined
        EXTERN  __iar_static_base$$GPREL
        .option push
        .option norelax
        lui  gp, %hi(__iar_static_base$$GPREL)
        addi gp, gp, %lo(__iar_static_base$$GPREL)
        .option pop

        CfiEnd  1

// ----------
// Init sp, note that this MAY be gp relaxed! (since if gp relaxations are
// allowed, __iar_cstart_init_gp is already done
        SECTION `.cstartup`:CODE:NOROOT(1)
        CfiBlk 2,__iar_program_start
        CODE
?cstart_init_sp:
        cfi ?RET Undefined
        lui     sp, %hi(SFE(CSTACK))
        addi    sp, sp, %lo(SFE(CSTACK))

        // Setup up a default interrupt handler to handle any exceptions that 
        // might occur during startup
        EXTERN __iar_default_minterrupt_handler
        lui     a0, %hi(__iar_default_minterrupt_handler)
        addi    a0, a0, %lo(__iar_default_minterrupt_handler)
        csrrci  x0, mtvec, 0x3
        csrs    mtvec, a0
        

        EXTWEAK __machine_interrupt_vector_setup
        CfiCall __machine_interrupt_vector_setup
        call    __machine_interrupt_vector_setup

__iar_program_start_metal:
#ifdef __riscv_flen
        // Enable the floating-point unit by setting the "fs" field in
        // the "mstatus" register.

        lui     a0, %hi(1 << 13)
        csrs    mstatus, a0

        // Set rounding mode to "round to nearest" and clear
        // the floating-point exception flags.
        csrwi   fcsr, 0
#endif

        EXTERN  __low_level_init
        EXTERN  __iar_data_init2

        CfiCall __low_level_init
        call    __low_level_init
        beq     a0, zero, ?cstart_call_main

        CfiCall __iar_data_init2
        call    __iar_data_init2

?cstart_call_main:
        EXTERN  SystemInit
        //li12    a0, 0                   ; argc
        CfiCall SystemInit
        call    SystemInit
        
        EXTERN  main
        li12    a0, 0                   ; argc
        CfiCall main
        call    main
        EXTERN  exit

        CfiCall exit
        call    exit
?cstart_end:
        j       ?cstart_end
        CfiEnd  2


        /* This section is required by some devices to handle HW reset */
        SECTION `.alias.hwreset`:CODE:NOROOT(2)
        PUBLIC __alias_hw_reset
__alias_hw_reset:
        csrci        mstatus, 0x08
        lui     a0, %hi(__iar_program_start)
        addi    a0, a0, %lo(__iar_program_start)
        jr       a0

        END

        End

