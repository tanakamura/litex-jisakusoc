	.comm	dummy, 64, 64


	.text

	.globl	touch_avx
touch_avx:
	vmovdqu	%ymm0, %ymm1
	ret

	.globl	do_vzeroupper
do_vzeroupper:
	vzeroupper
	ret

	.globl	do_vzeroall
do_vzeroall:
	vzeroall
	ret

	.globl	touch_x87
touch_x87:
	fldz
	ficompl	dummy(%rip)
	ret


	.globl	touch_mmx
touch_mmx:
	movq	%mm0, %mm1
	ret


	.globl	do_emms
do_emms:
	//emms
	ret


.macro	set_eaxedx
	movl	%esi, %eax
	shrq	$32, %rsi
	movl	%esi, %edx
.endm


	.globl	do_xsave
do_xsave:
	set_eaxedx
	xsave	(%rdi)
	ret

	.globl	do_xsaveopt
do_xsaveopt:
	set_eaxedx
	xsaveopt	(%rdi)
	ret

	.globl	do_xsavec
do_xsavec:
	set_eaxedx
	xsavec	(%rdi)
	ret

	.globl	do_xrstor
do_xrstor:
	set_eaxedx
	xrstor	(%rdi)
	ret


	.altmacro
 	.macro	getset_reg name, opc, from, to
	.globl	get_\name\()\from
	.globl	set_\name\()\from
get_\name\()\from:
	opc&	%name&from&, (%rdi)
	ret
set_\name\()\from:
	opc&	 (%rdi), %name&from&
	ret
        .if	to&-from&
	getset_reg	name&,opc&,%from&+1,to&
	.endif
	.endm

	getset_reg	mm, movq, 0, 7

	getset_reg	xmm, movups, 0, 15
	getset_reg	ymm, vmovups, 0, 15
	getset_reg	zmm, vmovups, 0, 15


