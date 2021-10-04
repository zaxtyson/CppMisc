	.file	"volatile.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	movl	counter(%rip), %eax
	cmpl	$10, %eax
	je	.L2
.L3:
	movl	counter(%rip), %eax
	addl	$1, %eax
	movl	%eax, counter(%rip)
	movl	counter(%rip), %eax
	cmpl	$10, %eax
	jne	.L3
.L2:
	movl	$0, %eax
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.local	counter
	.comm	counter,4,4
	.ident	"GCC: (SUSE Linux) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
