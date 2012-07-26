Created by the following command line:
arm-none-eabi-objdump --adjust-vma=0x1fffec00 -m arm -b binary -M force-thumb -D stmf051-sysmem.img

stmf051-sysmem.img:     file format binary


Disassembly of section .data:

1fffec00 <.data>:
1fffec00:	0678      	lsls	r0, r7, #25
1fffec02:	2000      	movs	r0, #0
1fffec04:	ee35 1fff 	mrc	15, 1, r1, cr5, cr15, {7}
1fffec08:	eced 1fff 	stcl	15, cr1, [sp], #1020	; 0x3fc
1fffec0c:	eced 1fff 	stcl	15, cr1, [sp], #1020	; 0x3fc
1fffec10:	f000 f802 	bl	0x1fffec18
1fffec14:	f000 f840 	bl	0x1fffec98
1fffec18:	a00c      	add	r0, pc, #48	; (adr r0, 0x1fffec4c)
1fffec1a:	c830      	ldmia	r0!, {r4, r5}
1fffec1c:	3808      	subs	r0, #8
1fffec1e:	1824      	adds	r4, r4, r0
1fffec20:	182d      	adds	r5, r5, r0
1fffec22:	46a2      	mov	sl, r4
1fffec24:	1e67      	subs	r7, r4, #1
1fffec26:	46ab      	mov	fp, r5
1fffec28:	4654      	mov	r4, sl
1fffec2a:	465d      	mov	r5, fp
1fffec2c:	42ac      	cmp	r4, r5
1fffec2e:	d101      	bne.n	0x1fffec34
1fffec30:	f000 f832 	bl	0x1fffec98
1fffec34:	467e      	mov	r6, pc
1fffec36:	3e0f      	subs	r6, #15
1fffec38:	cc0f      	ldmia	r4!, {r0, r1, r2, r3}
1fffec3a:	46b6      	mov	lr, r6
1fffec3c:	2601      	movs	r6, #1
1fffec3e:	4233      	tst	r3, r6
1fffec40:	d000      	beq.n	0x1fffec44
1fffec42:	1afb      	subs	r3, r7, r3
1fffec44:	46a2      	mov	sl, r4
1fffec46:	46ab      	mov	fp, r5
1fffec48:	4333      	orrs	r3, r6
1fffec4a:	4718      	bx	r3
1fffec4c:	0ae8      	lsrs	r0, r5, #11
1fffec4e:	0000      	movs	r0, r0
1fffec50:	0b08      	lsrs	r0, r1, #12
1fffec52:	0000      	movs	r0, r0
1fffec54:	3a10      	subs	r2, #16
1fffec56:	d302      	bcc.n	0x1fffec5e
1fffec58:	c878      	ldmia	r0!, {r3, r4, r5, r6}
1fffec5a:	c178      	stmia	r1!, {r3, r4, r5, r6}
1fffec5c:	d8fa      	bhi.n	0x1fffec54
1fffec5e:	0752      	lsls	r2, r2, #29
1fffec60:	d301      	bcc.n	0x1fffec66
1fffec62:	c830      	ldmia	r0!, {r4, r5}
1fffec64:	c130      	stmia	r1!, {r4, r5}
1fffec66:	d501      	bpl.n	0x1fffec6c
1fffec68:	6804      	ldr	r4, [r0, #0]
1fffec6a:	600c      	str	r4, [r1, #0]
1fffec6c:	4770      	bx	lr
1fffec6e:	0000      	movs	r0, r0
1fffec70:	2300      	movs	r3, #0
1fffec72:	2400      	movs	r4, #0
1fffec74:	2500      	movs	r5, #0
1fffec76:	2600      	movs	r6, #0
1fffec78:	3a10      	subs	r2, #16
1fffec7a:	d301      	bcc.n	0x1fffec80
1fffec7c:	c178      	stmia	r1!, {r3, r4, r5, r6}
1fffec7e:	d8fb      	bhi.n	0x1fffec78
1fffec80:	0752      	lsls	r2, r2, #29
1fffec82:	d300      	bcc.n	0x1fffec86
1fffec84:	c130      	stmia	r1!, {r4, r5}
1fffec86:	d500      	bpl.n	0x1fffec8a
1fffec88:	600b      	str	r3, [r1, #0]
1fffec8a:	4770      	bx	lr
1fffec8c:	b51f      	push	{r0, r1, r2, r3, r4, lr}
1fffec8e:	46c0      	nop			; (mov r8, r8)
1fffec90:	46c0      	nop			; (mov r8, r8)
1fffec92:	bd1f      	pop	{r0, r1, r2, r3, r4, pc}
1fffec94:	b510      	push	{r4, lr}
1fffec96:	bd10      	pop	{r4, pc}
1fffec98:	f000 fd1b 	bl	0x1ffff6d2
1fffec9c:	4611      	mov	r1, r2
1fffec9e:	f7ff fff5 	bl	0x1fffec8c
1fffeca2:	f000 f89c 	bl	0x1fffedde
1fffeca6:	f000 fd33 	bl	0x1ffff710
1fffecaa:	b403      	push	{r0, r1}
1fffecac:	f7ff fff2 	bl	0x1fffec94
1fffecb0:	bc03      	pop	{r0, r1}
1fffecb2:	f000 fd37 	bl	0x1ffff724
1fffecb6:	0000      	movs	r0, r0
1fffecb8:	f380 8808 	msr	MSP, r0
1fffecbc:	4770      	bx	lr
1fffecbe:	b672      	cpsid	i
1fffecc0:	4770      	bx	lr
1fffecc2:	b662      	cpsie	i
1fffecc4:	4770      	bx	lr
1fffecc6:	0000      	movs	r0, r0
1fffecc8:	4805      	ldr	r0, [pc, #20]	; (0x1fffece0)
1fffecca:	2100      	movs	r1, #0
1fffeccc:	6101      	str	r1, [r0, #16]
1fffecce:	6181      	str	r1, [r0, #24]
1fffecd0:	2101      	movs	r1, #1
1fffecd2:	04c9      	lsls	r1, r1, #19
1fffecd4:	6141      	str	r1, [r0, #20]
1fffecd6:	4770      	bx	lr
1fffecd8:	4903      	ldr	r1, [pc, #12]	; (0x1fffece8)
1fffecda:	4802      	ldr	r0, [pc, #8]	; (0x1fffece4)
1fffecdc:	60c8      	str	r0, [r1, #12]
1fffecde:	4770      	bx	lr
1fffece0:	e000      	b.n	0x1fffece4
1fffece2:	e000      	b.n	0x1fffece6
1fffece4:	0004      	movs	r4, r0
1fffece6:	05fa      	lsls	r2, r7, #23
1fffece8:	ed00 e000 	stc	0, cr14, [r0, #-0]
1fffecec:	b510      	push	{r4, lr}
1fffecee:	f7ff fff3 	bl	0x1fffecd8
1fffecf2:	bd10      	pop	{r4, pc}
1fffecf4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
1fffecf6:	4842      	ldr	r0, [pc, #264]	; (0x1fffee00)
1fffecf8:	4940      	ldr	r1, [pc, #256]	; (0x1fffedfc)
1fffecfa:	6001      	str	r1, [r0, #0]
1fffecfc:	2106      	movs	r1, #6
1fffecfe:	6041      	str	r1, [r0, #4]
1fffed00:	4940      	ldr	r1, [pc, #256]	; (0x1fffee04)
1fffed02:	6001      	str	r1, [r0, #0]
1fffed04:	f000 f8ca 	bl	0x1fffee9c
1fffed08:	4940      	ldr	r1, [pc, #256]	; (0x1fffee0c)
1fffed0a:	483f      	ldr	r0, [pc, #252]	; (0x1fffee08)
1fffed0c:	6088      	str	r0, [r1, #8]
1fffed0e:	f7ff ffdb 	bl	0x1fffecc8
1fffed12:	4e3f      	ldr	r6, [pc, #252]	; (0x1fffee10)
1fffed14:	2004      	movs	r0, #4
1fffed16:	6130      	str	r0, [r6, #16]
1fffed18:	0447      	lsls	r7, r0, #17
1fffed1a:	6177      	str	r7, [r6, #20]
1fffed1c:	0301      	lsls	r1, r0, #12
1fffed1e:	483d      	ldr	r0, [pc, #244]	; (0x1fffee14)
1fffed20:	6181      	str	r1, [r0, #24]
1fffed22:	00c9      	lsls	r1, r1, #3
1fffed24:	61c1      	str	r1, [r0, #28]
1fffed26:	493c      	ldr	r1, [pc, #240]	; (0x1fffee18)
1fffed28:	6141      	str	r1, [r0, #20]
1fffed2a:	f000 f902 	bl	0x1fffef32
1fffed2e:	2409      	movs	r4, #9
1fffed30:	483a      	ldr	r0, [pc, #232]	; (0x1fffee1c)
1fffed32:	06e4      	lsls	r4, r4, #27
1fffed34:	6020      	str	r0, [r4, #0]
1fffed36:	2000      	movs	r0, #0
1fffed38:	80a0      	strh	r0, [r4, #4]
1fffed3a:	4839      	ldr	r0, [pc, #228]	; (0x1fffee20)
1fffed3c:	60a0      	str	r0, [r4, #8]
1fffed3e:	4839      	ldr	r0, [pc, #228]	; (0x1fffee24)
1fffed40:	60e0      	str	r0, [r4, #12]
1fffed42:	4839      	ldr	r0, [pc, #228]	; (0x1fffee28)
1fffed44:	6260      	str	r0, [r4, #36]	; 0x24
1fffed46:	4d39      	ldr	r5, [pc, #228]	; (0x1fffee2c)
1fffed48:	8a20      	ldrh	r0, [r4, #16]
1fffed4a:	0540      	lsls	r0, r0, #21
1fffed4c:	d401      	bmi.n	0x1fffed52
1fffed4e:	2001      	movs	r0, #1
1fffed50:	60a8      	str	r0, [r5, #8]
1fffed52:	8a20      	ldrh	r0, [r4, #16]
1fffed54:	0bc0      	lsrs	r0, r0, #15
1fffed56:	d101      	bne.n	0x1fffed5c
1fffed58:	2001      	movs	r0, #1
1fffed5a:	60e8      	str	r0, [r5, #12]
1fffed5c:	68a8      	ldr	r0, [r5, #8]
1fffed5e:	2801      	cmp	r0, #1
1fffed60:	d11a      	bne.n	0x1fffed98
1fffed62:	8a20      	ldrh	r0, [r4, #16]
1fffed64:	0540      	lsls	r0, r0, #21
1fffed66:	d517      	bpl.n	0x1fffed98
1fffed68:	2005      	movs	r0, #5
1fffed6a:	6130      	str	r0, [r6, #16]
1fffed6c:	4830      	ldr	r0, [pc, #192]	; (0x1fffee30)
1fffed6e:	e000      	b.n	0x1fffed72
1fffed70:	1e40      	subs	r0, r0, #1
1fffed72:	8a21      	ldrh	r1, [r4, #16]
1fffed74:	0549      	lsls	r1, r1, #21
1fffed76:	d502      	bpl.n	0x1fffed7e
1fffed78:	2800      	cmp	r0, #0
1fffed7a:	d1f9      	bne.n	0x1fffed70
1fffed7c:	e008      	b.n	0x1fffed90
1fffed7e:	2800      	cmp	r0, #0
1fffed80:	d006      	beq.n	0x1fffed90
1fffed82:	69b0      	ldr	r0, [r6, #24]
1fffed84:	1a38      	subs	r0, r7, r0
1fffed86:	6068      	str	r0, [r5, #4]
1fffed88:	4828      	ldr	r0, [pc, #160]	; (0x1fffee2c)
1fffed8a:	3010      	adds	r0, #16
1fffed8c:	6800      	ldr	r0, [r0, #0]
1fffed8e:	e01c      	b.n	0x1fffedca
1fffed90:	2000      	movs	r0, #0
1fffed92:	60a8      	str	r0, [r5, #8]
1fffed94:	f7ff ff98 	bl	0x1fffecc8
1fffed98:	68e8      	ldr	r0, [r5, #12]
1fffed9a:	2801      	cmp	r0, #1
1fffed9c:	d1d4      	bne.n	0x1fffed48
1fffed9e:	8a20      	ldrh	r0, [r4, #16]
1fffeda0:	0bc0      	lsrs	r0, r0, #15
1fffeda2:	d0d1      	beq.n	0x1fffed48
1fffeda4:	2005      	movs	r0, #5
1fffeda6:	6130      	str	r0, [r6, #16]
1fffeda8:	4821      	ldr	r0, [pc, #132]	; (0x1fffee30)
1fffedaa:	e000      	b.n	0x1fffedae
1fffedac:	1e40      	subs	r0, r0, #1
1fffedae:	8a21      	ldrh	r1, [r4, #16]
1fffedb0:	0bc9      	lsrs	r1, r1, #15
1fffedb2:	d002      	beq.n	0x1fffedba
1fffedb4:	2800      	cmp	r0, #0
1fffedb6:	d1f9      	bne.n	0x1fffedac
1fffedb8:	e00c      	b.n	0x1fffedd4
1fffedba:	2800      	cmp	r0, #0
1fffedbc:	d00a      	beq.n	0x1fffedd4
1fffedbe:	69b0      	ldr	r0, [r6, #24]
1fffedc0:	1a38      	subs	r0, r7, r0
1fffedc2:	6068      	str	r0, [r5, #4]
1fffedc4:	4819      	ldr	r0, [pc, #100]	; (0x1fffee2c)
1fffedc6:	3010      	adds	r0, #16
1fffedc8:	6840      	ldr	r0, [r0, #4]
1fffedca:	6028      	str	r0, [r5, #0]
1fffedcc:	f7ff ff7c 	bl	0x1fffecc8
1fffedd0:	2001      	movs	r0, #1
1fffedd2:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
1fffedd4:	2000      	movs	r0, #0
1fffedd6:	60e8      	str	r0, [r5, #12]
1fffedd8:	f7ff ff76 	bl	0x1fffecc8
1fffeddc:	e7b4      	b.n	0x1fffed48
1fffedde:	f7ff ff6e 	bl	0x1fffecbe
1fffede2:	f7ff ff87 	bl	0x1fffecf4
1fffede6:	2801      	cmp	r0, #1
1fffede8:	d002      	beq.n	0x1fffedf0
1fffedea:	f7ff ff75 	bl	0x1fffecd8
1fffedee:	e7fe      	b.n	0x1fffedee
1fffedf0:	f000 f9a7 	bl	0x1ffff142
1fffedf4:	f000 fa26 	bl	0x1ffff244
1fffedf8:	e7f9      	b.n	0x1fffedee
1fffedfa:	0000      	movs	r0, r0
1fffedfc:	5555      	strb	r5, [r2, r5]
1fffedfe:	0000      	movs	r0, r0
1fffee00:	3000      	adds	r0, #0
1fffee02:	4000      	ands	r0, r0
1fffee04:	aaaa      	add	r2, sp, #680	; 0x2a8
1fffee06:	0000      	movs	r0, r0
1fffee08:	ec00 1fff 	stc	15, cr1, [r0], {255}	; 0xff
1fffee0c:	ed00 e000 	stc	0, cr14, [r0, #-0]
1fffee10:	e000      	b.n	0x1fffee14
1fffee12:	e000      	b.n	0x1fffee16
1fffee14:	1000      	asrs	r0, r0, #32
1fffee16:	4002      	ands	r2, r0
1fffee18:	0014      	movs	r4, r2
1fffee1a:	0002      	movs	r2, r0
1fffee1c:	0000      	movs	r0, r0
1fffee1e:	a828      	add	r0, sp, #160	; 0xa0
1fffee20:	0000      	movs	r0, r0
1fffee22:	fc3c 0000 	ldc2	0, cr0, [ip], #-0
1fffee26:	5414      	strb	r4, [r2, r0]
1fffee28:	0110      	lsls	r0, r2, #4
1fffee2a:	1100      	asrs	r0, r0, #4
1fffee2c:	0000      	movs	r0, r0
1fffee2e:	2000      	movs	r0, #0
1fffee30:	5fff      	ldrsh	r7, [r7, r7]
1fffee32:	0000      	movs	r0, r0
1fffee34:	4803      	ldr	r0, [pc, #12]	; (0x1fffee44)
1fffee36:	4700      	bx	r0
1fffee38:	4803      	ldr	r0, [pc, #12]	; (0x1fffee48)
1fffee3a:	4904      	ldr	r1, [pc, #16]	; (0x1fffee4c)
1fffee3c:	4a04      	ldr	r2, [pc, #16]	; (0x1fffee50)
1fffee3e:	4b05      	ldr	r3, [pc, #20]	; (0x1fffee54)
1fffee40:	4770      	bx	lr
1fffee42:	0000      	movs	r0, r0
1fffee44:	ec11 1fff 	ldc	15, cr1, [r1], {255}	; 0xff
1fffee48:	0078      	lsls	r0, r7, #1
1fffee4a:	2000      	movs	r0, #0
1fffee4c:	0678      	lsls	r0, r7, #25
1fffee4e:	2000      	movs	r0, #0
1fffee50:	0278      	lsls	r0, r7, #9
1fffee52:	2000      	movs	r0, #0
1fffee54:	0278      	lsls	r0, r7, #9
1fffee56:	2000      	movs	r0, #0
1fffee58:	491e      	ldr	r1, [pc, #120]	; (0x1fffeed4)
1fffee5a:	2001      	movs	r0, #1
1fffee5c:	7008      	strb	r0, [r1, #0]
1fffee5e:	2000      	movs	r0, #0
1fffee60:	8088      	strh	r0, [r1, #4]
1fffee62:	71c8      	strb	r0, [r1, #7]
1fffee64:	8048      	strh	r0, [r1, #2]
1fffee66:	7188      	strb	r0, [r1, #6]
1fffee68:	4a1a      	ldr	r2, [pc, #104]	; (0x1fffeed4)
1fffee6a:	3220      	adds	r2, #32
1fffee6c:	7310      	strb	r0, [r2, #12]
1fffee6e:	8210      	strh	r0, [r2, #16]
1fffee70:	2380      	movs	r3, #128	; 0x80
1fffee72:	7513      	strb	r3, [r2, #20]
1fffee74:	6088      	str	r0, [r1, #8]
1fffee76:	4770      	bx	lr
1fffee78:	b500      	push	{lr}
1fffee7a:	f7ff ffed 	bl	0x1fffee58
1fffee7e:	2200      	movs	r2, #0
1fffee80:	4814      	ldr	r0, [pc, #80]	; (0x1fffeed4)
1fffee82:	43d2      	mvns	r2, r2
1fffee84:	60c2      	str	r2, [r0, #12]
1fffee86:	2100      	movs	r1, #0
1fffee88:	60c1      	str	r1, [r0, #12]
1fffee8a:	6102      	str	r2, [r0, #16]
1fffee8c:	6101      	str	r1, [r0, #16]
1fffee8e:	6282      	str	r2, [r0, #40]	; 0x28
1fffee90:	6281      	str	r1, [r0, #40]	; 0x28
1fffee92:	6181      	str	r1, [r0, #24]
1fffee94:	61c1      	str	r1, [r0, #28]
1fffee96:	2114      	movs	r1, #20
1fffee98:	6141      	str	r1, [r0, #20]
1fffee9a:	bd00      	pop	{pc}
1fffee9c:	b570      	push	{r4, r5, r6, lr}
1fffee9e:	4c0e      	ldr	r4, [pc, #56]	; (0x1fffeed8)
1fffeea0:	f7ff ffda 	bl	0x1fffee58
1fffeea4:	490d      	ldr	r1, [pc, #52]	; (0x1fffeedc)
1fffeea6:	2000      	movs	r0, #0
1fffeea8:	6008      	str	r0, [r1, #0]
1fffeeaa:	4d0a      	ldr	r5, [pc, #40]	; (0x1fffeed4)
1fffeeac:	2010      	movs	r0, #16
1fffeeae:	71a8      	strb	r0, [r5, #6]
1fffeeb0:	2001      	movs	r0, #1
1fffeeb2:	70e8      	strb	r0, [r5, #3]
1fffeeb4:	e000      	b.n	0x1fffeeb8
1fffeeb6:	1e64      	subs	r4, r4, #1
1fffeeb8:	78e8      	ldrb	r0, [r5, #3]
1fffeeba:	2803      	cmp	r0, #3
1fffeebc:	d002      	beq.n	0x1fffeec4
1fffeebe:	2c00      	cmp	r4, #0
1fffeec0:	d1f9      	bne.n	0x1fffeeb6
1fffeec2:	e001      	b.n	0x1fffeec8
1fffeec4:	2c00      	cmp	r4, #0
1fffeec6:	d101      	bne.n	0x1fffeecc
1fffeec8:	f7ff ff06 	bl	0x1fffecd8
1fffeecc:	2002      	movs	r0, #2
1fffeece:	7128      	strb	r0, [r5, #4]
1fffeed0:	bd70      	pop	{r4, r5, r6, pc}
1fffeed2:	0000      	movs	r0, r0
1fffeed4:	1000      	asrs	r0, r0, #32
1fffeed6:	4002      	ands	r2, r0
1fffeed8:	ffff 0001 	vaddl.u<illegal width 64>	q8, d15, d1
1fffeedc:	2000      	movs	r0, #0
1fffeede:	4002      	ands	r2, r0
1fffeee0:	211f      	movs	r1, #31
1fffeee2:	06c9      	lsls	r1, r1, #27
1fffeee4:	2201      	movs	r2, #1
1fffeee6:	1841      	adds	r1, r0, r1
1fffeee8:	0412      	lsls	r2, r2, #16
1fffeeea:	4291      	cmp	r1, r2
1fffeeec:	d201      	bcs.n	0x1fffeef2
1fffeeee:	2002      	movs	r0, #2
1fffeef0:	4770      	bx	lr
1fffeef2:	4987      	ldr	r1, [pc, #540]	; (0x1ffff110)
1fffeef4:	2203      	movs	r2, #3
1fffeef6:	1841      	adds	r1, r0, r1
1fffeef8:	02d2      	lsls	r2, r2, #11
1fffeefa:	4291      	cmp	r1, r2
1fffeefc:	d201      	bcs.n	0x1fffef02
1fffeefe:	2003      	movs	r0, #3
1fffef00:	4770      	bx	lr
1fffef02:	4984      	ldr	r1, [pc, #528]	; (0x1ffff114)
1fffef04:	1841      	adds	r1, r0, r1
1fffef06:	290f      	cmp	r1, #15
1fffef08:	d201      	bcs.n	0x1fffef0e
1fffef0a:	2004      	movs	r0, #4
1fffef0c:	4770      	bx	lr
1fffef0e:	4982      	ldr	r1, [pc, #520]	; (0x1ffff118)
1fffef10:	1840      	adds	r0, r0, r1
1fffef12:	4982      	ldr	r1, [pc, #520]	; (0x1ffff11c)
1fffef14:	4288      	cmp	r0, r1
1fffef16:	d201      	bcs.n	0x1fffef1c
1fffef18:	2005      	movs	r0, #5
1fffef1a:	4770      	bx	lr
1fffef1c:	2001      	movs	r0, #1
1fffef1e:	4770      	bx	lr
1fffef20:	487f      	ldr	r0, [pc, #508]	; (0x1ffff120)
1fffef22:	2200      	movs	r2, #0
1fffef24:	2101      	movs	r1, #1
1fffef26:	0749      	lsls	r1, r1, #29
1fffef28:	6002      	str	r2, [r0, #0]
1fffef2a:	1f00      	subs	r0, r0, #4
1fffef2c:	4288      	cmp	r0, r1
1fffef2e:	d1fb      	bne.n	0x1fffef28
1fffef30:	4770      	bx	lr
1fffef32:	487d      	ldr	r0, [pc, #500]	; (0x1ffff128)
1fffef34:	497b      	ldr	r1, [pc, #492]	; (0x1ffff124)
1fffef36:	6041      	str	r1, [r0, #4]
1fffef38:	497c      	ldr	r1, [pc, #496]	; (0x1ffff12c)
1fffef3a:	6041      	str	r1, [r0, #4]
1fffef3c:	4770      	bx	lr
1fffef3e:	487a      	ldr	r0, [pc, #488]	; (0x1ffff128)
1fffef40:	4978      	ldr	r1, [pc, #480]	; (0x1ffff124)
1fffef42:	6081      	str	r1, [r0, #8]
1fffef44:	4979      	ldr	r1, [pc, #484]	; (0x1ffff12c)
1fffef46:	6081      	str	r1, [r0, #8]
1fffef48:	4770      	bx	lr
1fffef4a:	b510      	push	{r4, lr}
1fffef4c:	4878      	ldr	r0, [pc, #480]	; (0x1ffff130)
1fffef4e:	4976      	ldr	r1, [pc, #472]	; (0x1ffff128)
1fffef50:	e000      	b.n	0x1fffef54
1fffef52:	1e40      	subs	r0, r0, #1
1fffef54:	68ca      	ldr	r2, [r1, #12]
1fffef56:	07d2      	lsls	r2, r2, #31
1fffef58:	d002      	beq.n	0x1fffef60
1fffef5a:	2800      	cmp	r0, #0
1fffef5c:	d1f9      	bne.n	0x1fffef52
1fffef5e:	e001      	b.n	0x1fffef64
1fffef60:	2800      	cmp	r0, #0
1fffef62:	d101      	bne.n	0x1fffef68
1fffef64:	f7ff feb8 	bl	0x1fffecd8
1fffef68:	bd10      	pop	{r4, pc}
1fffef6a:	b570      	push	{r4, r5, r6, lr}
1fffef6c:	4606      	mov	r6, r0
1fffef6e:	460d      	mov	r5, r1
1fffef70:	f7ff ffeb 	bl	0x1fffef4a
1fffef74:	4c6c      	ldr	r4, [pc, #432]	; (0x1ffff128)
1fffef76:	6920      	ldr	r0, [r4, #16]
1fffef78:	2101      	movs	r1, #1
1fffef7a:	4308      	orrs	r0, r1
1fffef7c:	6120      	str	r0, [r4, #16]
1fffef7e:	8035      	strh	r5, [r6, #0]
1fffef80:	f7ff ffe3 	bl	0x1fffef4a
1fffef84:	68e0      	ldr	r0, [r4, #12]
1fffef86:	0740      	lsls	r0, r0, #29
1fffef88:	d403      	bmi.n	0x1fffef92
1fffef8a:	0c28      	lsrs	r0, r5, #16
1fffef8c:	8070      	strh	r0, [r6, #2]
1fffef8e:	f7ff ffdc 	bl	0x1fffef4a
1fffef92:	68e0      	ldr	r0, [r4, #12]
1fffef94:	0740      	lsls	r0, r0, #29
1fffef96:	d503      	bpl.n	0x1fffefa0
1fffef98:	68e0      	ldr	r0, [r4, #12]
1fffef9a:	2104      	movs	r1, #4
1fffef9c:	4308      	orrs	r0, r1
1fffef9e:	60e0      	str	r0, [r4, #12]
1fffefa0:	6920      	ldr	r0, [r4, #16]
1fffefa2:	0840      	lsrs	r0, r0, #1
1fffefa4:	0040      	lsls	r0, r0, #1
1fffefa6:	6120      	str	r0, [r4, #16]
1fffefa8:	bd70      	pop	{r4, r5, r6, pc}
1fffefaa:	b570      	push	{r4, r5, r6, lr}
1fffefac:	4605      	mov	r5, r0
1fffefae:	f7ff ffcc 	bl	0x1fffef4a
1fffefb2:	4c5d      	ldr	r4, [pc, #372]	; (0x1ffff128)
1fffefb4:	2002      	movs	r0, #2
1fffefb6:	6120      	str	r0, [r4, #16]
1fffefb8:	6165      	str	r5, [r4, #20]
1fffefba:	2042      	movs	r0, #66	; 0x42
1fffefbc:	6120      	str	r0, [r4, #16]
1fffefbe:	f7ff ffc4 	bl	0x1fffef4a
1fffefc2:	2000      	movs	r0, #0
1fffefc4:	6120      	str	r0, [r4, #16]
1fffefc6:	bd70      	pop	{r4, r5, r6, pc}
1fffefc8:	b570      	push	{r4, r5, r6, lr}
1fffefca:	f7ff ffb8 	bl	0x1fffef3e
1fffefce:	f7ff ffbc 	bl	0x1fffef4a
1fffefd2:	4c55      	ldr	r4, [pc, #340]	; (0x1ffff128)
1fffefd4:	6920      	ldr	r0, [r4, #16]
1fffefd6:	2504      	movs	r5, #4
1fffefd8:	4328      	orrs	r0, r5
1fffefda:	6120      	str	r0, [r4, #16]
1fffefdc:	6920      	ldr	r0, [r4, #16]
1fffefde:	2140      	movs	r1, #64	; 0x40
1fffefe0:	4308      	orrs	r0, r1
1fffefe2:	6120      	str	r0, [r4, #16]
1fffefe4:	f7ff ffb1 	bl	0x1fffef4a
1fffefe8:	6920      	ldr	r0, [r4, #16]
1fffefea:	43a8      	bics	r0, r5
1fffefec:	6120      	str	r0, [r4, #16]
1fffefee:	bd70      	pop	{r4, r5, r6, pc}
1fffeff0:	b570      	push	{r4, r5, r6, lr}
1fffeff2:	f7ff ffa4 	bl	0x1fffef3e
1fffeff6:	f7ff ffa8 	bl	0x1fffef4a
1fffeffa:	4c4b      	ldr	r4, [pc, #300]	; (0x1ffff128)
1fffeffc:	6920      	ldr	r0, [r4, #16]
1fffeffe:	2520      	movs	r5, #32
1ffff000:	4328      	orrs	r0, r5
1ffff002:	6120      	str	r0, [r4, #16]
1ffff004:	6920      	ldr	r0, [r4, #16]
1ffff006:	2140      	movs	r1, #64	; 0x40
1ffff008:	4308      	orrs	r0, r1
1ffff00a:	6120      	str	r0, [r4, #16]
1ffff00c:	f7ff ff9d 	bl	0x1fffef4a
1ffff010:	6920      	ldr	r0, [r4, #16]
1ffff012:	43a8      	bics	r0, r5
1ffff014:	6120      	str	r0, [r4, #16]
1ffff016:	bd70      	pop	{r4, r5, r6, pc}
1ffff018:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
1ffff01a:	4c3e      	ldr	r4, [pc, #248]	; (0x1ffff114)
1ffff01c:	4264      	negs	r4, r4
1ffff01e:	8827      	ldrh	r7, [r4, #0]
1ffff020:	8861      	ldrh	r1, [r4, #2]
1ffff022:	43c0      	mvns	r0, r0
1ffff024:	b2c5      	uxtb	r5, r0
1ffff026:	0400      	lsls	r0, r0, #16
1ffff028:	0e06      	lsrs	r6, r0, #24
1ffff02a:	9100      	str	r1, [sp, #0]
1ffff02c:	f7ff ffe0 	bl	0x1fffeff0
1ffff030:	f7ff ff7f 	bl	0x1fffef32
1ffff034:	f7ff ff83 	bl	0x1fffef3e
1ffff038:	483b      	ldr	r0, [pc, #236]	; (0x1ffff128)
1ffff03a:	6901      	ldr	r1, [r0, #16]
1ffff03c:	2210      	movs	r2, #16
1ffff03e:	4311      	orrs	r1, r2
1ffff040:	6101      	str	r1, [r0, #16]
1ffff042:	b2f8      	uxtb	r0, r7
1ffff044:	8020      	strh	r0, [r4, #0]
1ffff046:	f7ff ff80 	bl	0x1fffef4a
1ffff04a:	9900      	ldr	r1, [sp, #0]
1ffff04c:	b2c8      	uxtb	r0, r1
1ffff04e:	8060      	strh	r0, [r4, #2]
1ffff050:	f7ff ff7b 	bl	0x1fffef4a
1ffff054:	2dff      	cmp	r5, #255	; 0xff
1ffff056:	d002      	beq.n	0x1ffff05e
1ffff058:	8125      	strh	r5, [r4, #8]
1ffff05a:	f7ff ff76 	bl	0x1fffef4a
1ffff05e:	2eff      	cmp	r6, #255	; 0xff
1ffff060:	d002      	beq.n	0x1ffff068
1ffff062:	8166      	strh	r6, [r4, #10]
1ffff064:	f7ff ff71 	bl	0x1fffef4a
1ffff068:	482f      	ldr	r0, [pc, #188]	; (0x1ffff128)
1ffff06a:	6902      	ldr	r2, [r0, #16]
1ffff06c:	2110      	movs	r1, #16
1ffff06e:	438a      	bics	r2, r1
1ffff070:	6102      	str	r2, [r0, #16]
1ffff072:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
1ffff074:	4770      	bx	lr
1ffff076:	b5f1      	push	{r0, r4, r5, r6, r7, lr}
1ffff078:	b082      	sub	sp, #8
1ffff07a:	f7ff ff66 	bl	0x1fffef4a
1ffff07e:	4c25      	ldr	r4, [pc, #148]	; (0x1ffff114)
1ffff080:	4264      	negs	r4, r4
1ffff082:	8867      	ldrh	r7, [r4, #2]
1ffff084:	88a5      	ldrh	r5, [r4, #4]
1ffff086:	88e6      	ldrh	r6, [r4, #6]
1ffff088:	8920      	ldrh	r0, [r4, #8]
1ffff08a:	9000      	str	r0, [sp, #0]
1ffff08c:	8960      	ldrh	r0, [r4, #10]
1ffff08e:	9001      	str	r0, [sp, #4]
1ffff090:	f7ff ffae 	bl	0x1fffeff0
1ffff094:	f7ff ff4d 	bl	0x1fffef32
1ffff098:	f7ff ff51 	bl	0x1fffef3e
1ffff09c:	f7ff ff55 	bl	0x1fffef4a
1ffff0a0:	4821      	ldr	r0, [pc, #132]	; (0x1ffff128)
1ffff0a2:	6901      	ldr	r1, [r0, #16]
1ffff0a4:	2210      	movs	r2, #16
1ffff0a6:	4311      	orrs	r1, r2
1ffff0a8:	6101      	str	r1, [r0, #16]
1ffff0aa:	9802      	ldr	r0, [sp, #8]
1ffff0ac:	2800      	cmp	r0, #0
1ffff0ae:	d017      	beq.n	0x1ffff0e0
1ffff0b0:	b2f8      	uxtb	r0, r7
1ffff0b2:	8060      	strh	r0, [r4, #2]
1ffff0b4:	f7ff ff49 	bl	0x1fffef4a
1ffff0b8:	b2e8      	uxtb	r0, r5
1ffff0ba:	80a0      	strh	r0, [r4, #4]
1ffff0bc:	f7ff ff45 	bl	0x1fffef4a
1ffff0c0:	b2f0      	uxtb	r0, r6
1ffff0c2:	80e0      	strh	r0, [r4, #6]
1ffff0c4:	f7ff ff41 	bl	0x1fffef4a
1ffff0c8:	9800      	ldr	r0, [sp, #0]
1ffff0ca:	b2c0      	uxtb	r0, r0
1ffff0cc:	8120      	strh	r0, [r4, #8]
1ffff0ce:	f7ff ff3c 	bl	0x1fffef4a
1ffff0d2:	9801      	ldr	r0, [sp, #4]
1ffff0d4:	b2c0      	uxtb	r0, r0
1ffff0d6:	8120      	strh	r0, [r4, #8]
1ffff0d8:	f7ff ff37 	bl	0x1fffef4a
1ffff0dc:	2000      	movs	r0, #0
1ffff0de:	e000      	b.n	0x1ffff0e2
1ffff0e0:	20aa      	movs	r0, #170	; 0xaa
1ffff0e2:	8020      	strh	r0, [r4, #0]
1ffff0e4:	f7ff ff31 	bl	0x1fffef4a
1ffff0e8:	480f      	ldr	r0, [pc, #60]	; (0x1ffff128)
1ffff0ea:	6902      	ldr	r2, [r0, #16]
1ffff0ec:	2110      	movs	r1, #16
1ffff0ee:	438a      	bics	r2, r1
1ffff0f0:	6102      	str	r2, [r0, #16]
1ffff0f2:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
1ffff0f4:	490c      	ldr	r1, [pc, #48]	; (0x1ffff128)
1ffff0f6:	2000      	movs	r0, #0
1ffff0f8:	69c9      	ldr	r1, [r1, #28]
1ffff0fa:	0789      	lsls	r1, r1, #30
1ffff0fc:	d500      	bpl.n	0x1ffff100
1ffff0fe:	2001      	movs	r0, #1
1ffff100:	4770      	bx	lr
1ffff102:	4809      	ldr	r0, [pc, #36]	; (0x1ffff128)
1ffff104:	6901      	ldr	r1, [r0, #16]
1ffff106:	2201      	movs	r2, #1
1ffff108:	0352      	lsls	r2, r2, #13
1ffff10a:	4311      	orrs	r1, r2
1ffff10c:	6101      	str	r1, [r0, #16]
1ffff10e:	4770      	bx	lr
1ffff110:	f800 dfff 	strb.w	sp, [r0, #255]!
1ffff114:	0800      	lsrs	r0, r0, #32
1ffff116:	e000      	b.n	0x1ffff11a
1ffff118:	1400      	asrs	r0, r0, #16
1ffff11a:	e000      	b.n	0x1ffff11e
1ffff11c:	0bff      	lsrs	r7, r7, #15
1ffff11e:	0000      	movs	r0, r0
1ffff120:	1ffc      	subs	r4, r7, #7
1ffff122:	2000      	movs	r0, #0
1ffff124:	0123      	lsls	r3, r4, #4
1ffff126:	4567      	cmp	r7, ip
1ffff128:	2000      	movs	r0, #0
1ffff12a:	4002      	ands	r2, r0
1ffff12c:	89ab      	ldrh	r3, [r5, #12]
1ffff12e:	cdef      	ldmia	r5, {r0, r1, r2, r3, r5, r6, r7}
1ffff130:	0fff      	lsrs	r7, r7, #31
1ffff132:	0000      	movs	r0, r0
1ffff134:	49f9      	ldr	r1, [pc, #996]	; (0x1ffff51c)
1ffff136:	6809      	ldr	r1, [r1, #0]
1ffff138:	8508      	strh	r0, [r1, #40]	; 0x28
1ffff13a:	69c8      	ldr	r0, [r1, #28]
1ffff13c:	0640      	lsls	r0, r0, #25
1ffff13e:	d5fc      	bpl.n	0x1ffff13a
1ffff140:	4770      	bx	lr
1ffff142:	b510      	push	{r4, lr}
1ffff144:	f7ff fdbb 	bl	0x1fffecbe
1ffff148:	4cf5      	ldr	r4, [pc, #980]	; (0x1ffff520)
1ffff14a:	2107      	movs	r1, #7
1ffff14c:	6820      	ldr	r0, [r4, #0]
1ffff14e:	f000 fa10 	bl	0x1ffff572
1ffff152:	4af2      	ldr	r2, [pc, #968]	; (0x1ffff51c)
1ffff154:	6811      	ldr	r1, [r2, #0]
1ffff156:	8188      	strh	r0, [r1, #12]
1ffff158:	48f2      	ldr	r0, [pc, #968]	; (0x1ffff524)
1ffff15a:	6008      	str	r0, [r1, #0]
1ffff15c:	2079      	movs	r0, #121	; 0x79
1ffff15e:	f7ff ffe9 	bl	0x1ffff134
1ffff162:	6810      	ldr	r0, [r2, #0]
1ffff164:	8c80      	ldrh	r0, [r0, #36]	; 0x24
1ffff166:	6020      	str	r0, [r4, #0]
1ffff168:	bd10      	pop	{r4, pc}
1ffff16a:	48ec      	ldr	r0, [pc, #944]	; (0x1ffff51c)
1ffff16c:	49ee      	ldr	r1, [pc, #952]	; (0x1ffff528)
1ffff16e:	6800      	ldr	r0, [r0, #0]
1ffff170:	4aee      	ldr	r2, [pc, #952]	; (0x1ffff52c)
1ffff172:	e000      	b.n	0x1ffff176
1ffff174:	6011      	str	r1, [r2, #0]
1ffff176:	69c3      	ldr	r3, [r0, #28]
1ffff178:	069b      	lsls	r3, r3, #26
1ffff17a:	d5fb      	bpl.n	0x1ffff174
1ffff17c:	8c80      	ldrh	r0, [r0, #36]	; 0x24
1ffff17e:	b2c0      	uxtb	r0, r0
1ffff180:	4770      	bx	lr
1ffff182:	b570      	push	{r4, r5, r6, lr}
1ffff184:	460e      	mov	r6, r1
1ffff186:	1c45      	adds	r5, r0, #1
1ffff188:	4604      	mov	r4, r0
1ffff18a:	d006      	beq.n	0x1ffff19a
1ffff18c:	f7ff ffed 	bl	0x1ffff16a
1ffff190:	4044      	eors	r4, r0
1ffff192:	7030      	strb	r0, [r6, #0]
1ffff194:	1c76      	adds	r6, r6, #1
1ffff196:	1e6d      	subs	r5, r5, #1
1ffff198:	d1f8      	bne.n	0x1ffff18c
1ffff19a:	f7ff ffe6 	bl	0x1ffff16a
1ffff19e:	42a0      	cmp	r0, r4
1ffff1a0:	d004      	beq.n	0x1ffff1ac
1ffff1a2:	201f      	movs	r0, #31
1ffff1a4:	f7ff ffc6 	bl	0x1ffff134
1ffff1a8:	2055      	movs	r0, #85	; 0x55
1ffff1aa:	bd70      	pop	{r4, r5, r6, pc}
1ffff1ac:	20aa      	movs	r0, #170	; 0xaa
1ffff1ae:	bd70      	pop	{r4, r5, r6, pc}
1ffff1b0:	b5f0      	push	{r4, r5, r6, r7, lr}
1ffff1b2:	460e      	mov	r6, r1
1ffff1b4:	0a01      	lsrs	r1, r0, #8
1ffff1b6:	b2c4      	uxtb	r4, r0
1ffff1b8:	1c45      	adds	r5, r0, #1
1ffff1ba:	404c      	eors	r4, r1
1ffff1bc:	2d00      	cmp	r5, #0
1ffff1be:	d00d      	beq.n	0x1ffff1dc
1ffff1c0:	f7ff ffd3 	bl	0x1ffff16a
1ffff1c4:	4627      	mov	r7, r4
1ffff1c6:	4047      	eors	r7, r0
1ffff1c8:	0204      	lsls	r4, r0, #8
1ffff1ca:	f7ff ffce 	bl	0x1ffff16a
1ffff1ce:	4320      	orrs	r0, r4
1ffff1d0:	b2c4      	uxtb	r4, r0
1ffff1d2:	407c      	eors	r4, r7
1ffff1d4:	8030      	strh	r0, [r6, #0]
1ffff1d6:	1cb6      	adds	r6, r6, #2
1ffff1d8:	1e6d      	subs	r5, r5, #1
1ffff1da:	d1f1      	bne.n	0x1ffff1c0
1ffff1dc:	f7ff ffc5 	bl	0x1ffff16a
1ffff1e0:	42a0      	cmp	r0, r4
1ffff1e2:	d004      	beq.n	0x1ffff1ee
1ffff1e4:	201f      	movs	r0, #31
1ffff1e6:	f7ff ffa5 	bl	0x1ffff134
1ffff1ea:	2055      	movs	r0, #85	; 0x55
1ffff1ec:	bdf0      	pop	{r4, r5, r6, r7, pc}
1ffff1ee:	20aa      	movs	r0, #170	; 0xaa
1ffff1f0:	bdf0      	pop	{r4, r5, r6, r7, pc}
1ffff1f2:	b570      	push	{r4, r5, r6, lr}
1ffff1f4:	f7ff ff7e 	bl	0x1ffff0f4
1ffff1f8:	2800      	cmp	r0, #0
1ffff1fa:	d11e      	bne.n	0x1ffff23a
1ffff1fc:	2079      	movs	r0, #121	; 0x79
1ffff1fe:	f7ff ff99 	bl	0x1ffff134
1ffff202:	f7ff ffb2 	bl	0x1ffff16a
1ffff206:	4604      	mov	r4, r0
1ffff208:	0605      	lsls	r5, r0, #24
1ffff20a:	f7ff ffae 	bl	0x1ffff16a
1ffff20e:	0406      	lsls	r6, r0, #16
1ffff210:	432e      	orrs	r6, r5
1ffff212:	4625      	mov	r5, r4
1ffff214:	4045      	eors	r5, r0
1ffff216:	f7ff ffa8 	bl	0x1ffff16a
1ffff21a:	0204      	lsls	r4, r0, #8
1ffff21c:	4334      	orrs	r4, r6
1ffff21e:	4045      	eors	r5, r0
1ffff220:	f7ff ffa3 	bl	0x1ffff16a
1ffff224:	4304      	orrs	r4, r0
1ffff226:	4045      	eors	r5, r0
1ffff228:	f7ff ff9f 	bl	0x1ffff16a
1ffff22c:	42a8      	cmp	r0, r5
1ffff22e:	d104      	bne.n	0x1ffff23a
1ffff230:	2079      	movs	r0, #121	; 0x79
1ffff232:	f7ff ff7f 	bl	0x1ffff134
1ffff236:	4620      	mov	r0, r4
1ffff238:	bd70      	pop	{r4, r5, r6, pc}
1ffff23a:	201f      	movs	r0, #31
1ffff23c:	f7ff ff7a 	bl	0x1ffff134
1ffff240:	48bb      	ldr	r0, [pc, #748]	; (0x1ffff530)
1ffff242:	bd70      	pop	{r4, r5, r6, pc}
1ffff244:	a0bb      	add	r0, pc, #748	; (adr r0, 0x1ffff534)
1ffff246:	b0ff      	sub	sp, #508	; 0x1fc
1ffff248:	b0ff      	sub	sp, #508	; 0x1fc
1ffff24a:	6803      	ldr	r3, [r0, #0]
1ffff24c:	6842      	ldr	r2, [r0, #4]
1ffff24e:	6881      	ldr	r1, [r0, #8]
1ffff250:	b088      	sub	sp, #32
1ffff252:	68c0      	ldr	r0, [r0, #12]
1ffff254:	9300      	str	r3, [sp, #0]
1ffff256:	9201      	str	r2, [sp, #4]
1ffff258:	9102      	str	r1, [sp, #8]
1ffff25a:	9003      	str	r0, [sp, #12]
1ffff25c:	49b3      	ldr	r1, [pc, #716]	; (0x1ffff52c)
1ffff25e:	48b2      	ldr	r0, [pc, #712]	; (0x1ffff528)
1ffff260:	6008      	str	r0, [r1, #0]
1ffff262:	f7ff ff82 	bl	0x1ffff16a
1ffff266:	4604      	mov	r4, r0
1ffff268:	f7ff ff7f 	bl	0x1ffff16a
1ffff26c:	4601      	mov	r1, r0
1ffff26e:	4061      	eors	r1, r4
1ffff270:	29ff      	cmp	r1, #255	; 0xff
1ffff272:	d000      	beq.n	0x1ffff276
1ffff274:	2455      	movs	r4, #85	; 0x55
1ffff276:	2c31      	cmp	r4, #49	; 0x31
1ffff278:	d072      	beq.n	0x1ffff360
1ffff27a:	dc0c      	bgt.n	0x1ffff296
1ffff27c:	2c02      	cmp	r4, #2
1ffff27e:	d02b      	beq.n	0x1ffff2d8
1ffff280:	dc04      	bgt.n	0x1ffff28c
1ffff282:	2c00      	cmp	r4, #0
1ffff284:	d014      	beq.n	0x1ffff2b0
1ffff286:	2c01      	cmp	r4, #1
1ffff288:	d169      	bne.n	0x1ffff35e
1ffff28a:	e01a      	b.n	0x1ffff2c2
1ffff28c:	2c11      	cmp	r4, #17
1ffff28e:	d033      	beq.n	0x1ffff2f8
1ffff290:	2c21      	cmp	r4, #33	; 0x21
1ffff292:	d1f9      	bne.n	0x1ffff288
1ffff294:	e04e      	b.n	0x1ffff334
1ffff296:	2c73      	cmp	r4, #115	; 0x73
1ffff298:	d07a      	beq.n	0x1ffff390
1ffff29a:	dc04      	bgt.n	0x1ffff2a6
1ffff29c:	2c44      	cmp	r4, #68	; 0x44
1ffff29e:	d078      	beq.n	0x1ffff392
1ffff2a0:	2c63      	cmp	r4, #99	; 0x63
1ffff2a2:	d1f1      	bne.n	0x1ffff288
1ffff2a4:	e0fe      	b.n	0x1ffff4a4
1ffff2a6:	2c82      	cmp	r4, #130	; 0x82
1ffff2a8:	d074      	beq.n	0x1ffff394
1ffff2aa:	2c92      	cmp	r4, #146	; 0x92
1ffff2ac:	d1ec      	bne.n	0x1ffff288
1ffff2ae:	e154      	b.n	0x1ffff55a
1ffff2b0:	2300      	movs	r3, #0
1ffff2b2:	466a      	mov	r2, sp
1ffff2b4:	5cd0      	ldrb	r0, [r2, r3]
1ffff2b6:	f7ff ff3d 	bl	0x1ffff134
1ffff2ba:	1c5b      	adds	r3, r3, #1
1ffff2bc:	2b0f      	cmp	r3, #15
1ffff2be:	d3f9      	bcc.n	0x1ffff2b4
1ffff2c0:	e7cc      	b.n	0x1ffff25c
1ffff2c2:	2079      	movs	r0, #121	; 0x79
1ffff2c4:	f7ff ff36 	bl	0x1ffff134
1ffff2c8:	2031      	movs	r0, #49	; 0x31
1ffff2ca:	f7ff ff33 	bl	0x1ffff134
1ffff2ce:	2000      	movs	r0, #0
1ffff2d0:	f7ff ff30 	bl	0x1ffff134
1ffff2d4:	2000      	movs	r0, #0
1ffff2d6:	e009      	b.n	0x1ffff2ec
1ffff2d8:	2079      	movs	r0, #121	; 0x79
1ffff2da:	f7ff ff2b 	bl	0x1ffff134
1ffff2de:	2001      	movs	r0, #1
1ffff2e0:	f7ff ff28 	bl	0x1ffff134
1ffff2e4:	2004      	movs	r0, #4
1ffff2e6:	f7ff ff25 	bl	0x1ffff134
1ffff2ea:	2040      	movs	r0, #64	; 0x40
1ffff2ec:	f7ff ff22 	bl	0x1ffff134
1ffff2f0:	e0d6      	b.n	0x1ffff4a0
1ffff2f2:	f7ff ff1f 	bl	0x1ffff134
1ffff2f6:	e7b1      	b.n	0x1ffff25c
1ffff2f8:	f7ff ff7b 	bl	0x1ffff1f2
1ffff2fc:	4605      	mov	r5, r0
1ffff2fe:	f7ff fdef 	bl	0x1fffeee0
1ffff302:	b2c0      	uxtb	r0, r0
1ffff304:	2801      	cmp	r0, #1
1ffff306:	d02a      	beq.n	0x1ffff35e
1ffff308:	f7ff ff2f 	bl	0x1ffff16a
1ffff30c:	4604      	mov	r4, r0
1ffff30e:	43e0      	mvns	r0, r4
1ffff310:	b2c6      	uxtb	r6, r0
1ffff312:	f7ff ff2a 	bl	0x1ffff16a
1ffff316:	42b0      	cmp	r0, r6
1ffff318:	d121      	bne.n	0x1ffff35e
1ffff31a:	2079      	movs	r0, #121	; 0x79
1ffff31c:	f7ff ff0a 	bl	0x1ffff134
1ffff320:	1c64      	adds	r4, r4, #1
1ffff322:	d09b      	beq.n	0x1ffff25c
1ffff324:	4628      	mov	r0, r5
1ffff326:	1c6d      	adds	r5, r5, #1
1ffff328:	7800      	ldrb	r0, [r0, #0]
1ffff32a:	f7ff ff03 	bl	0x1ffff134
1ffff32e:	1e64      	subs	r4, r4, #1
1ffff330:	d1f8      	bne.n	0x1ffff324
1ffff332:	e793      	b.n	0x1ffff25c
1ffff334:	f7ff ff5d 	bl	0x1ffff1f2
1ffff338:	4604      	mov	r4, r0
1ffff33a:	f7ff fdd1 	bl	0x1fffeee0
1ffff33e:	b2c0      	uxtb	r0, r0
1ffff340:	2802      	cmp	r0, #2
1ffff342:	d001      	beq.n	0x1ffff348
1ffff344:	2803      	cmp	r0, #3
1ffff346:	d10a      	bne.n	0x1ffff35e
1ffff348:	f7ff fd96 	bl	0x1fffee78
1ffff34c:	f7ff fcb9 	bl	0x1fffecc2
1ffff350:	6865      	ldr	r5, [r4, #4]
1ffff352:	6820      	ldr	r0, [r4, #0]
1ffff354:	f7ff fcb0 	bl	0x1fffecb8
1ffff358:	47a8      	blx	r5
1ffff35a:	e77f      	b.n	0x1ffff25c
1ffff35c:	e000      	b.n	0x1ffff360
1ffff35e:	e086      	b.n	0x1ffff46e
1ffff360:	f7ff ff47 	bl	0x1ffff1f2
1ffff364:	4607      	mov	r7, r0
1ffff366:	f7ff fdbb 	bl	0x1fffeee0
1ffff36a:	b2c0      	uxtb	r0, r0
1ffff36c:	9004      	str	r0, [sp, #16]
1ffff36e:	2801      	cmp	r0, #1
1ffff370:	d07e      	beq.n	0x1ffff470
1ffff372:	f7ff fefa 	bl	0x1ffff16a
1ffff376:	a905      	add	r1, sp, #20
1ffff378:	1c44      	adds	r4, r0, #1
1ffff37a:	460e      	mov	r6, r1
1ffff37c:	f7ff ff01 	bl	0x1ffff182
1ffff380:	28aa      	cmp	r0, #170	; 0xaa
1ffff382:	d19d      	bne.n	0x1ffff2c0
1ffff384:	9804      	ldr	r0, [sp, #16]
1ffff386:	2802      	cmp	r0, #2
1ffff388:	d005      	beq.n	0x1ffff396
1ffff38a:	2803      	cmp	r0, #3
1ffff38c:	d018      	beq.n	0x1ffff3c0
1ffff38e:	e013      	b.n	0x1ffff3b8
1ffff390:	e0ab      	b.n	0x1ffff4ea
1ffff392:	e050      	b.n	0x1ffff436
1ffff394:	e0b6      	b.n	0x1ffff504
1ffff396:	07a0      	lsls	r0, r4, #30
1ffff398:	d002      	beq.n	0x1ffff3a0
1ffff39a:	20fc      	movs	r0, #252	; 0xfc
1ffff39c:	4004      	ands	r4, r0
1ffff39e:	1d24      	adds	r4, r4, #4
1ffff3a0:	f7ff fdc7 	bl	0x1fffef32
1ffff3a4:	0025      	movs	r5, r4
1ffff3a6:	d007      	beq.n	0x1ffff3b8
1ffff3a8:	1b70      	subs	r0, r6, r5
1ffff3aa:	5901      	ldr	r1, [r0, r4]
1ffff3ac:	1b78      	subs	r0, r7, r5
1ffff3ae:	1900      	adds	r0, r0, r4
1ffff3b0:	f7ff fddb 	bl	0x1fffef6a
1ffff3b4:	1f2d      	subs	r5, r5, #4
1ffff3b6:	d1f7      	bne.n	0x1ffff3a8
1ffff3b8:	9804      	ldr	r0, [sp, #16]
1ffff3ba:	2804      	cmp	r0, #4
1ffff3bc:	d00e      	beq.n	0x1ffff3dc
1ffff3be:	e06f      	b.n	0x1ffff4a0
1ffff3c0:	07a0      	lsls	r0, r4, #30
1ffff3c2:	d002      	beq.n	0x1ffff3ca
1ffff3c4:	20fc      	movs	r0, #252	; 0xfc
1ffff3c6:	4004      	ands	r4, r0
1ffff3c8:	1d24      	adds	r4, r4, #4
1ffff3ca:	0020      	movs	r0, r4
1ffff3cc:	d068      	beq.n	0x1ffff4a0
1ffff3ce:	1a31      	subs	r1, r6, r0
1ffff3d0:	5909      	ldr	r1, [r1, r4]
1ffff3d2:	1a3a      	subs	r2, r7, r0
1ffff3d4:	5111      	str	r1, [r2, r4]
1ffff3d6:	1f00      	subs	r0, r0, #4
1ffff3d8:	d1f9      	bne.n	0x1ffff3ce
1ffff3da:	e061      	b.n	0x1ffff4a0
1ffff3dc:	4859      	ldr	r0, [pc, #356]	; (0x1ffff544)
1ffff3de:	4287      	cmp	r7, r0
1ffff3e0:	d15e      	bne.n	0x1ffff4a0
1ffff3e2:	07e0      	lsls	r0, r4, #31
1ffff3e4:	d002      	beq.n	0x1ffff3ec
1ffff3e6:	20fe      	movs	r0, #254	; 0xfe
1ffff3e8:	4004      	ands	r4, r0
1ffff3ea:	1ca4      	adds	r4, r4, #2
1ffff3ec:	f7ff fe00 	bl	0x1fffeff0
1ffff3f0:	f7ff fd9f 	bl	0x1fffef32
1ffff3f4:	f7ff fda3 	bl	0x1fffef3e
1ffff3f8:	4853      	ldr	r0, [pc, #332]	; (0x1ffff548)
1ffff3fa:	6901      	ldr	r1, [r0, #16]
1ffff3fc:	2210      	movs	r2, #16
1ffff3fe:	4311      	orrs	r1, r2
1ffff400:	6101      	str	r1, [r0, #16]
1ffff402:	f7ff fda2 	bl	0x1fffef4a
1ffff406:	0025      	movs	r5, r4
1ffff408:	d008      	beq.n	0x1ffff41c
1ffff40a:	1b70      	subs	r0, r6, r5
1ffff40c:	5b00      	ldrh	r0, [r0, r4]
1ffff40e:	1b79      	subs	r1, r7, r5
1ffff410:	b2c0      	uxtb	r0, r0
1ffff412:	5308      	strh	r0, [r1, r4]
1ffff414:	f7ff fd99 	bl	0x1fffef4a
1ffff418:	1ead      	subs	r5, r5, #2
1ffff41a:	d1f6      	bne.n	0x1ffff40a
1ffff41c:	484a      	ldr	r0, [pc, #296]	; (0x1ffff548)
1ffff41e:	6901      	ldr	r1, [r0, #16]
1ffff420:	2210      	movs	r2, #16
1ffff422:	4391      	bics	r1, r2
1ffff424:	6101      	str	r1, [r0, #16]
1ffff426:	2079      	movs	r0, #121	; 0x79
1ffff428:	f7ff fe84 	bl	0x1ffff134
1ffff42c:	f7ff fe69 	bl	0x1ffff102
1ffff430:	f7ff fc52 	bl	0x1fffecd8
1ffff434:	e034      	b.n	0x1ffff4a0
1ffff436:	f7ff fe5d 	bl	0x1ffff0f4
1ffff43a:	2801      	cmp	r0, #1
1ffff43c:	d018      	beq.n	0x1ffff470
1ffff43e:	2079      	movs	r0, #121	; 0x79
1ffff440:	f7ff fe78 	bl	0x1ffff134
1ffff444:	f7ff fe91 	bl	0x1ffff16a
1ffff448:	4604      	mov	r4, r0
1ffff44a:	f7ff fe8e 	bl	0x1ffff16a
1ffff44e:	0221      	lsls	r1, r4, #8
1ffff450:	4308      	orrs	r0, r1
1ffff452:	4a3e      	ldr	r2, [pc, #248]	; (0x1ffff54c)
1ffff454:	0901      	lsrs	r1, r0, #4
1ffff456:	4291      	cmp	r1, r2
1ffff458:	d10f      	bne.n	0x1ffff47a
1ffff45a:	493d      	ldr	r1, [pc, #244]	; (0x1ffff550)
1ffff45c:	4288      	cmp	r0, r1
1ffff45e:	d109      	bne.n	0x1ffff474
1ffff460:	f7ff fe83 	bl	0x1ffff16a
1ffff464:	2800      	cmp	r0, #0
1ffff466:	d103      	bne.n	0x1ffff470
1ffff468:	f7ff fdae 	bl	0x1fffefc8
1ffff46c:	e018      	b.n	0x1ffff4a0
1ffff46e:	e7ff      	b.n	0x1ffff470
1ffff470:	201f      	movs	r0, #31
1ffff472:	e73e      	b.n	0x1ffff2f2
1ffff474:	f7ff fe79 	bl	0x1ffff16a
1ffff478:	e7fa      	b.n	0x1ffff470
1ffff47a:	a905      	add	r1, sp, #20
1ffff47c:	1c44      	adds	r4, r0, #1
1ffff47e:	460d      	mov	r5, r1
1ffff480:	f7ff fe96 	bl	0x1ffff1b0
1ffff484:	28aa      	cmp	r0, #170	; 0xaa
1ffff486:	d1f3      	bne.n	0x1ffff470
1ffff488:	2c00      	cmp	r4, #0
1ffff48a:	d009      	beq.n	0x1ffff4a0
1ffff48c:	2601      	movs	r6, #1
1ffff48e:	06f6      	lsls	r6, r6, #27
1ffff490:	8828      	ldrh	r0, [r5, #0]
1ffff492:	0280      	lsls	r0, r0, #10
1ffff494:	1980      	adds	r0, r0, r6
1ffff496:	f7ff fd88 	bl	0x1fffefaa
1ffff49a:	1cad      	adds	r5, r5, #2
1ffff49c:	1e64      	subs	r4, r4, #1
1ffff49e:	d1f7      	bne.n	0x1ffff490
1ffff4a0:	2079      	movs	r0, #121	; 0x79
1ffff4a2:	e726      	b.n	0x1ffff2f2
1ffff4a4:	f7ff fe26 	bl	0x1ffff0f4
1ffff4a8:	2801      	cmp	r0, #1
1ffff4aa:	d0e1      	beq.n	0x1ffff470
1ffff4ac:	2079      	movs	r0, #121	; 0x79
1ffff4ae:	f7ff fe41 	bl	0x1ffff134
1ffff4b2:	f7ff fe5a 	bl	0x1ffff16a
1ffff4b6:	a905      	add	r1, sp, #20
1ffff4b8:	1c44      	adds	r4, r0, #1
1ffff4ba:	460d      	mov	r5, r1
1ffff4bc:	f7ff fe61 	bl	0x1ffff182
1ffff4c0:	28aa      	cmp	r0, #170	; 0xaa
1ffff4c2:	d000      	beq.n	0x1ffff4c6
1ffff4c4:	e6ca      	b.n	0x1ffff25c
1ffff4c6:	2600      	movs	r6, #0
1ffff4c8:	2c00      	cmp	r4, #0
1ffff4ca:	d008      	beq.n	0x1ffff4de
1ffff4cc:	2001      	movs	r0, #1
1ffff4ce:	4629      	mov	r1, r5
1ffff4d0:	780a      	ldrb	r2, [r1, #0]
1ffff4d2:	1c6d      	adds	r5, r5, #1
1ffff4d4:	4601      	mov	r1, r0
1ffff4d6:	4091      	lsls	r1, r2
1ffff4d8:	430e      	orrs	r6, r1
1ffff4da:	1e64      	subs	r4, r4, #1
1ffff4dc:	d1f7      	bne.n	0x1ffff4ce
1ffff4de:	f7ff fd28 	bl	0x1fffef32
1ffff4e2:	4630      	mov	r0, r6
1ffff4e4:	f7ff fd98 	bl	0x1ffff018
1ffff4e8:	e008      	b.n	0x1ffff4fc
1ffff4ea:	f7ff fe03 	bl	0x1ffff0f4
1ffff4ee:	2801      	cmp	r0, #1
1ffff4f0:	d0be      	beq.n	0x1ffff470
1ffff4f2:	2079      	movs	r0, #121	; 0x79
1ffff4f4:	f7ff fe1e 	bl	0x1ffff134
1ffff4f8:	2000      	movs	r0, #0
1ffff4fa:	e00b      	b.n	0x1ffff514
1ffff4fc:	2079      	movs	r0, #121	; 0x79
1ffff4fe:	f7ff fe19 	bl	0x1ffff134
1ffff502:	e027      	b.n	0x1ffff554
1ffff504:	f7ff fdf6 	bl	0x1ffff0f4
1ffff508:	2801      	cmp	r0, #1
1ffff50a:	d0b1      	beq.n	0x1ffff470
1ffff50c:	2079      	movs	r0, #121	; 0x79
1ffff50e:	f7ff fe11 	bl	0x1ffff134
1ffff512:	2001      	movs	r0, #1
1ffff514:	f7ff fdaf 	bl	0x1ffff076
1ffff518:	e7f0      	b.n	0x1ffff4fc
1ffff51a:	0000      	movs	r0, r0
1ffff51c:	0000      	movs	r0, r0
1ffff51e:	2000      	movs	r0, #0
1ffff520:	0004      	movs	r4, r0
1ffff522:	2000      	movs	r0, #0
1ffff524:	140d      	asrs	r5, r1, #16
1ffff526:	0000      	movs	r0, r0
1ffff528:	aaaa      	add	r2, sp, #680	; 0x2a8
1ffff52a:	0000      	movs	r0, r0
1ffff52c:	3000      	adds	r0, #0
1ffff52e:	4000      	ands	r0, r0
1ffff530:	5555      	strb	r5, [r2, r5]
1ffff532:	5555      	strb	r5, [r2, r5]
1ffff534:	0b79      	lsrs	r1, r7, #13
1ffff536:	0031      	movs	r1, r6
1ffff538:	0201      	lsls	r1, r0, #8
1ffff53a:	2111      	movs	r1, #17
1ffff53c:	4431      	add	r1, r6
1ffff53e:	7363      	strb	r3, [r4, #13]
1ffff540:	9282      	str	r2, [sp, #520]	; 0x208
1ffff542:	0079      	lsls	r1, r7, #1
1ffff544:	f800 1fff 	strb.w	r1, [r0, #255]!
1ffff548:	2000      	movs	r0, #0
1ffff54a:	4002      	ands	r2, r0
1ffff54c:	0fff      	lsrs	r7, r7, #31
1ffff54e:	0000      	movs	r0, r0
1ffff550:	ffff 0000 	vaddl.u<illegal width 64>	q8, d15, d0
1ffff554:	f7ff fdd5 	bl	0x1ffff102
1ffff558:	e680      	b.n	0x1ffff25c
1ffff55a:	2079      	movs	r0, #121	; 0x79
1ffff55c:	f7ff fdea 	bl	0x1ffff134
1ffff560:	2000      	movs	r0, #0
1ffff562:	f7ff fd88 	bl	0x1ffff076
1ffff566:	2079      	movs	r0, #121	; 0x79
1ffff568:	f7ff fde4 	bl	0x1ffff134
1ffff56c:	f7ff fcd8 	bl	0x1fffef20
1ffff570:	e7f0      	b.n	0x1ffff554
1ffff572:	2200      	movs	r2, #0
1ffff574:	0903      	lsrs	r3, r0, #4
1ffff576:	428b      	cmp	r3, r1
1ffff578:	d32c      	bcc.n	0x1ffff5d4
1ffff57a:	0a03      	lsrs	r3, r0, #8
1ffff57c:	428b      	cmp	r3, r1
1ffff57e:	d311      	bcc.n	0x1ffff5a4
1ffff580:	2300      	movs	r3, #0
1ffff582:	469c      	mov	ip, r3
1ffff584:	e04e      	b.n	0x1ffff624
1ffff586:	4603      	mov	r3, r0
1ffff588:	430b      	orrs	r3, r1
1ffff58a:	d43c      	bmi.n	0x1ffff606
1ffff58c:	2200      	movs	r2, #0
1ffff58e:	0843      	lsrs	r3, r0, #1
1ffff590:	428b      	cmp	r3, r1
1ffff592:	d331      	bcc.n	0x1ffff5f8
1ffff594:	0903      	lsrs	r3, r0, #4
1ffff596:	428b      	cmp	r3, r1
1ffff598:	d31c      	bcc.n	0x1ffff5d4
1ffff59a:	0a03      	lsrs	r3, r0, #8
1ffff59c:	428b      	cmp	r3, r1
1ffff59e:	d301      	bcc.n	0x1ffff5a4
1ffff5a0:	4694      	mov	ip, r2
1ffff5a2:	e03f      	b.n	0x1ffff624
1ffff5a4:	09c3      	lsrs	r3, r0, #7
1ffff5a6:	428b      	cmp	r3, r1
1ffff5a8:	d301      	bcc.n	0x1ffff5ae
1ffff5aa:	01cb      	lsls	r3, r1, #7
1ffff5ac:	1ac0      	subs	r0, r0, r3
1ffff5ae:	4152      	adcs	r2, r2
1ffff5b0:	0983      	lsrs	r3, r0, #6
1ffff5b2:	428b      	cmp	r3, r1
1ffff5b4:	d301      	bcc.n	0x1ffff5ba
1ffff5b6:	018b      	lsls	r3, r1, #6
1ffff5b8:	1ac0      	subs	r0, r0, r3
1ffff5ba:	4152      	adcs	r2, r2
1ffff5bc:	0943      	lsrs	r3, r0, #5
1ffff5be:	428b      	cmp	r3, r1
1ffff5c0:	d301      	bcc.n	0x1ffff5c6
1ffff5c2:	014b      	lsls	r3, r1, #5
1ffff5c4:	1ac0      	subs	r0, r0, r3
1ffff5c6:	4152      	adcs	r2, r2
1ffff5c8:	0903      	lsrs	r3, r0, #4
1ffff5ca:	428b      	cmp	r3, r1
1ffff5cc:	d301      	bcc.n	0x1ffff5d2
1ffff5ce:	010b      	lsls	r3, r1, #4
1ffff5d0:	1ac0      	subs	r0, r0, r3
1ffff5d2:	4152      	adcs	r2, r2
1ffff5d4:	08c3      	lsrs	r3, r0, #3
1ffff5d6:	428b      	cmp	r3, r1
1ffff5d8:	d301      	bcc.n	0x1ffff5de
1ffff5da:	00cb      	lsls	r3, r1, #3
1ffff5dc:	1ac0      	subs	r0, r0, r3
1ffff5de:	4152      	adcs	r2, r2
1ffff5e0:	0883      	lsrs	r3, r0, #2
1ffff5e2:	428b      	cmp	r3, r1
1ffff5e4:	d301      	bcc.n	0x1ffff5ea
1ffff5e6:	008b      	lsls	r3, r1, #2
1ffff5e8:	1ac0      	subs	r0, r0, r3
1ffff5ea:	4152      	adcs	r2, r2
1ffff5ec:	0843      	lsrs	r3, r0, #1
1ffff5ee:	428b      	cmp	r3, r1
1ffff5f0:	d301      	bcc.n	0x1ffff5f6
1ffff5f2:	004b      	lsls	r3, r1, #1
1ffff5f4:	1ac0      	subs	r0, r0, r3
1ffff5f6:	4152      	adcs	r2, r2
1ffff5f8:	1a41      	subs	r1, r0, r1
1ffff5fa:	d200      	bcs.n	0x1ffff5fe
1ffff5fc:	4601      	mov	r1, r0
1ffff5fe:	4152      	adcs	r2, r2
1ffff600:	4610      	mov	r0, r2
1ffff602:	4770      	bx	lr
1ffff604:	e05d      	b.n	0x1ffff6c2
1ffff606:	0fca      	lsrs	r2, r1, #31
1ffff608:	d000      	beq.n	0x1ffff60c
1ffff60a:	4249      	negs	r1, r1
1ffff60c:	1003      	asrs	r3, r0, #32
1ffff60e:	d300      	bcc.n	0x1ffff612
1ffff610:	4240      	negs	r0, r0
1ffff612:	4053      	eors	r3, r2
1ffff614:	2200      	movs	r2, #0
1ffff616:	469c      	mov	ip, r3
1ffff618:	0903      	lsrs	r3, r0, #4
1ffff61a:	428b      	cmp	r3, r1
1ffff61c:	d32d      	bcc.n	0x1ffff67a
1ffff61e:	0a03      	lsrs	r3, r0, #8
1ffff620:	428b      	cmp	r3, r1
1ffff622:	d312      	bcc.n	0x1ffff64a
1ffff624:	22fc      	movs	r2, #252	; 0xfc
1ffff626:	0189      	lsls	r1, r1, #6
1ffff628:	ba12      	rev	r2, r2
1ffff62a:	0a03      	lsrs	r3, r0, #8
1ffff62c:	428b      	cmp	r3, r1
1ffff62e:	d30c      	bcc.n	0x1ffff64a
1ffff630:	0189      	lsls	r1, r1, #6
1ffff632:	1192      	asrs	r2, r2, #6
1ffff634:	428b      	cmp	r3, r1
1ffff636:	d308      	bcc.n	0x1ffff64a
1ffff638:	0189      	lsls	r1, r1, #6
1ffff63a:	1192      	asrs	r2, r2, #6
1ffff63c:	428b      	cmp	r3, r1
1ffff63e:	d304      	bcc.n	0x1ffff64a
1ffff640:	0189      	lsls	r1, r1, #6
1ffff642:	d03a      	beq.n	0x1ffff6ba
1ffff644:	1192      	asrs	r2, r2, #6
1ffff646:	e000      	b.n	0x1ffff64a
1ffff648:	0989      	lsrs	r1, r1, #6
1ffff64a:	09c3      	lsrs	r3, r0, #7
1ffff64c:	428b      	cmp	r3, r1
1ffff64e:	d301      	bcc.n	0x1ffff654
1ffff650:	01cb      	lsls	r3, r1, #7
1ffff652:	1ac0      	subs	r0, r0, r3
1ffff654:	4152      	adcs	r2, r2
1ffff656:	0983      	lsrs	r3, r0, #6
1ffff658:	428b      	cmp	r3, r1
1ffff65a:	d301      	bcc.n	0x1ffff660
1ffff65c:	018b      	lsls	r3, r1, #6
1ffff65e:	1ac0      	subs	r0, r0, r3
1ffff660:	4152      	adcs	r2, r2
1ffff662:	0943      	lsrs	r3, r0, #5
1ffff664:	428b      	cmp	r3, r1
1ffff666:	d301      	bcc.n	0x1ffff66c
1ffff668:	014b      	lsls	r3, r1, #5
1ffff66a:	1ac0      	subs	r0, r0, r3
1ffff66c:	4152      	adcs	r2, r2
1ffff66e:	0903      	lsrs	r3, r0, #4
1ffff670:	428b      	cmp	r3, r1
1ffff672:	d301      	bcc.n	0x1ffff678
1ffff674:	010b      	lsls	r3, r1, #4
1ffff676:	1ac0      	subs	r0, r0, r3
1ffff678:	4152      	adcs	r2, r2
1ffff67a:	08c3      	lsrs	r3, r0, #3
1ffff67c:	428b      	cmp	r3, r1
1ffff67e:	d301      	bcc.n	0x1ffff684
1ffff680:	00cb      	lsls	r3, r1, #3
1ffff682:	1ac0      	subs	r0, r0, r3
1ffff684:	4152      	adcs	r2, r2
1ffff686:	0883      	lsrs	r3, r0, #2
1ffff688:	428b      	cmp	r3, r1
1ffff68a:	d301      	bcc.n	0x1ffff690
1ffff68c:	008b      	lsls	r3, r1, #2
1ffff68e:	1ac0      	subs	r0, r0, r3
1ffff690:	4152      	adcs	r2, r2
1ffff692:	d2d9      	bcs.n	0x1ffff648
1ffff694:	0843      	lsrs	r3, r0, #1
1ffff696:	428b      	cmp	r3, r1
1ffff698:	d301      	bcc.n	0x1ffff69e
1ffff69a:	004b      	lsls	r3, r1, #1
1ffff69c:	1ac0      	subs	r0, r0, r3
1ffff69e:	4152      	adcs	r2, r2
1ffff6a0:	1a41      	subs	r1, r0, r1
1ffff6a2:	d200      	bcs.n	0x1ffff6a6
1ffff6a4:	4601      	mov	r1, r0
1ffff6a6:	4663      	mov	r3, ip
1ffff6a8:	4152      	adcs	r2, r2
1ffff6aa:	105b      	asrs	r3, r3, #1
1ffff6ac:	4610      	mov	r0, r2
1ffff6ae:	d301      	bcc.n	0x1ffff6b4
1ffff6b0:	4240      	negs	r0, r0
1ffff6b2:	2b00      	cmp	r3, #0
1ffff6b4:	d500      	bpl.n	0x1ffff6b8
1ffff6b6:	4249      	negs	r1, r1
1ffff6b8:	4770      	bx	lr
1ffff6ba:	4663      	mov	r3, ip
1ffff6bc:	105b      	asrs	r3, r3, #1
1ffff6be:	d300      	bcc.n	0x1ffff6c2
1ffff6c0:	4240      	negs	r0, r0
1ffff6c2:	b501      	push	{r0, lr}
1ffff6c4:	2000      	movs	r0, #0
1ffff6c6:	46c0      	nop			; (mov r8, r8)
1ffff6c8:	46c0      	nop			; (mov r8, r8)
1ffff6ca:	bd02      	pop	{r1, pc}
1ffff6cc:	4770      	bx	lr
1ffff6ce:	4770      	bx	lr
1ffff6d0:	4770      	bx	lr
1ffff6d2:	4675      	mov	r5, lr
1ffff6d4:	f000 f822 	bl	0x1ffff71c
1ffff6d8:	46ae      	mov	lr, r5
1ffff6da:	0005      	movs	r5, r0
1ffff6dc:	4669      	mov	r1, sp
1ffff6de:	4653      	mov	r3, sl
1ffff6e0:	08c0      	lsrs	r0, r0, #3
1ffff6e2:	00c0      	lsls	r0, r0, #3
1ffff6e4:	4685      	mov	sp, r0
1ffff6e6:	b018      	add	sp, #96	; 0x60
1ffff6e8:	b520      	push	{r5, lr}
1ffff6ea:	f7ff fba5 	bl	0x1fffee38
1ffff6ee:	bc60      	pop	{r5, r6}
1ffff6f0:	2700      	movs	r7, #0
1ffff6f2:	0849      	lsrs	r1, r1, #1
1ffff6f4:	46b6      	mov	lr, r6
1ffff6f6:	2600      	movs	r6, #0
1ffff6f8:	c5c0      	stmia	r5!, {r6, r7}
1ffff6fa:	c5c0      	stmia	r5!, {r6, r7}
1ffff6fc:	c5c0      	stmia	r5!, {r6, r7}
1ffff6fe:	c5c0      	stmia	r5!, {r6, r7}
1ffff700:	c5c0      	stmia	r5!, {r6, r7}
1ffff702:	c5c0      	stmia	r5!, {r6, r7}
1ffff704:	c5c0      	stmia	r5!, {r6, r7}
1ffff706:	c5c0      	stmia	r5!, {r6, r7}
1ffff708:	3d40      	subs	r5, #64	; 0x40
1ffff70a:	0049      	lsls	r1, r1, #1
1ffff70c:	468d      	mov	sp, r1
1ffff70e:	4770      	bx	lr
1ffff710:	4604      	mov	r4, r0
1ffff712:	46c0      	nop			; (mov r8, r8)
1ffff714:	46c0      	nop			; (mov r8, r8)
1ffff716:	4620      	mov	r0, r4
1ffff718:	f7ff fac7 	bl	0x1fffecaa
1ffff71c:	4800      	ldr	r0, [pc, #0]	; (0x1ffff720)
1ffff71e:	4770      	bx	lr
1ffff720:	0018      	movs	r0, r3
1ffff722:	2000      	movs	r0, #0
1ffff724:	4901      	ldr	r1, [pc, #4]	; (0x1ffff72c)
1ffff726:	2018      	movs	r0, #24
1ffff728:	beab      	bkpt	0x00ab
1ffff72a:	e7fe      	b.n	0x1ffff72a
1ffff72c:	0026      	movs	r6, r4
1ffff72e:	0002      	movs	r2, r0
1ffff730:	4770      	bx	lr
1ffff732:	0000      	movs	r0, r0
1ffff734:	f754 1fff 			; <UNDEFINED> instruction: 0xf7541fff
1ffff738:	0000      	movs	r0, r0
1ffff73a:	2000      	movs	r0, #0
1ffff73c:	0018      	movs	r0, r3
1ffff73e:	0000      	movs	r0, r0
1ffff740:	ec54 1fff 	mrrc	15, 15, r1, r4, cr15
1ffff744:	f76c 1fff 			; <UNDEFINED> instruction: 0xf76c1fff
1ffff748:	0018      	movs	r0, r3
1ffff74a:	2000      	movs	r0, #0
1ffff74c:	0660      	lsls	r0, r4, #25
1ffff74e:	0000      	movs	r0, r0
1ffff750:	ec70 1fff 	ldcl	15, cr1, [r0], #-1020	; 0xfffffc04
1ffff754:	3800      	subs	r0, #0
1ffff756:	4001      	ands	r1, r0
	...
1ffff764:	3800      	subs	r0, #0
1ffff766:	4001      	ands	r1, r0
1ffff768:	4400      	add	r0, r0
1ffff76a:	4000      	ands	r0, r0
1ffff76c:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff770:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff774:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff778:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff77c:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff780:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff784:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff788:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff78c:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff790:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff794:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff798:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff79c:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7a0:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7a4:	0031      	movs	r1, r6
1ffff7a6:	0021      	movs	r1, r4
1ffff7a8:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7ac:	0048      	lsls	r0, r1, #1
1ffff7ae:	0055      	lsls	r5, r2, #1
1ffff7b0:	5717      	ldrsb	r7, [r2, r4]
1ffff7b2:	4136      	asrs	r6, r6
1ffff7b4:	3336      	adds	r3, #54	; 0x36
1ffff7b6:	2032      	movs	r0, #50	; 0x32
1ffff7b8:	06db      	lsls	r3, r3, #27
1ffff7ba:	05f8      	lsls	r0, r7, #23
1ffff7bc:	fffc ffff 			; <UNDEFINED> instruction: 0xfffcffff
1ffff7c0:	ffff 0537 	vsli.32	d16, d23, #31
1ffff7c4:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7c8:	fffc ffff 			; <UNDEFINED> instruction: 0xfffcffff
1ffff7cc:	0040      	lsls	r0, r0, #1
1ffff7ce:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7d2:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7d6:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7da:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7de:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7e2:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7e6:	ffff fff3 			; <UNDEFINED> instruction: 0xfffffff3
1ffff7ea:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7ee:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7f2:	ffff ffff 			; <UNDEFINED> instruction: 0xffffffff
1ffff7f6:	ffff 7788 	vcvt.u<illegal width 64>.f<illegal width 64>	d23, d8
1ffff7fa:	aa55      	add	r2, sp, #340	; 0x154
1ffff7fc:	bd42      	pop	{r1, r6, pc}
1ffff7fe:	00ff      	lsls	r7, r7, #3
