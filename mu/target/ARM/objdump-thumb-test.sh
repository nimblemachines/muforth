# Test disassembly of Thumb code via ARM objdump by creating a file with an
# exhaustive set of Thumb instructions (not quite exhaustive in the 32-bit
# case, but at least _representative_), assembled with arm-none-eabi-as and
# disassembled with arm-none-eabi-objdump.

# Create asm file
./muforth -f target/ARM/all-thumbs-gas.mu4 cr bye > all-thumb16.s

# Assemble
echo Assembling
arm-none-eabi-as -o all-thumb16.o all-thumb16.s

# Disassemble
echo Disassembling
arm-none-eabi-objdump -d all-thumb16.o > all-thumb16.objdump
