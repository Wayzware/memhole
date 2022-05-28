	.file	"tester.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"/dev/memhole"
.LC1:
	.string	"%d|%d|%d\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB54:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	$4, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	movq	%fs:40, %rax
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	call	malloc@PLT
	movl	$1, %esi
	leaq	.LC0(%rip), %rdi
	movl	$666666, 4(%rsp)
	movl	$10, (%rax)
	movq	%rax, %rbx
	xorl	%eax, %eax
	call	open@PLT
	xorl	%edx, %edx
	movq	%rbx, %rsi
	movl	%eax, %ebp
	movl	%eax, %edi
	call	lseek64@PLT
	leaq	4(%rsp), %rsi
	movl	$4, %edx
	movl	%ebp, %edi
	call	write@PLT
	movl	(%rbx), %ecx
	cmpl	$10, %ecx
	jne	.L2
.L3:
	jmp	.L3
	.p2align 4,,10
	.p2align 3
.L2:
	movq	%rax, %r8
	movl	%ebp, %edx
	leaq	.LC1(%rip), %rsi
	xorl	%eax, %eax
	movl	$1, %edi
	call	__printf_chk@PLT
	movl	%ebp, %edi
	call	close@PLT
	movq	8(%rsp), %rax
	xorq	%fs:40, %rax
	jne	.L7
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L7:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE54:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
