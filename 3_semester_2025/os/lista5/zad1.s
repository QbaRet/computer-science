	.file	"zad1.c"
	.text
	.globl	put_item
	.type	put_item, @function
put_item:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rdi
	call	pthread_mutex_lock@PLT
	movq	-8(%rbp), %rax
	movq	(%rax), %rdx
	movq	-8(%rbp), %rax
	movl	12(%rax), %eax
	cltq
	salq	$2, %rax
	addq	%rax, %rdx
	movl	-12(%rbp), %eax
	movl	%eax, (%rdx)
	movq	-8(%rbp), %rax
	movl	12(%rax), %eax
	leal	1(%rax), %ecx
	movq	-8(%rbp), %rax
	movl	8(%rax), %esi
	movl	%ecx, %eax
	cltd
	idivl	%esi
	movl	%edx, %ecx
	movl	%ecx, %edx
	movq	-8(%rbp), %rax
	movl	%edx, 12(%rax)
	movq	-8(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rdi
	call	pthread_mutex_unlock@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	put_item, .-put_item
	.globl	get_item
	.type	get_item, @function
get_item:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rdi
	call	pthread_mutex_lock@PLT
	movq	-24(%rbp), %rax
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	movl	16(%rax), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -4(%rbp)
	movq	-24(%rbp), %rax
	movl	16(%rax), %eax
	leal	1(%rax), %ecx
	movq	-24(%rbp), %rax
	movl	8(%rax), %esi
	movl	%ecx, %eax
	cltd
	idivl	%esi
	movl	%edx, %ecx
	movl	%ecx, %edx
	movq	-24(%rbp), %rax
	movl	%edx, 16(%rax)
	movq	-24(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rdi
	call	pthread_mutex_unlock@PLT
	movl	-4(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	get_item, .-get_item
	.section	.rodata
.LC0:
	.string	"Wyprodukowano: %d\n"
	.text
	.globl	producer
	.type	producer, @function
producer:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	movl	%eax, -16(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L5
.L6:
	call	rand@PLT
	movslq	%eax, %rdx
	imulq	$1374389535, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$5, %edx
	movl	%eax, %ecx
	sarl	$31, %ecx
	subl	%ecx, %edx
	movl	%edx, -12(%rbp)
	movl	-12(%rbp), %edx
	imull	$100, %edx, %edx
	subl	%edx, %eax
	movl	%eax, -12(%rbp)
	movq	-8(%rbp), %rax
	addq	$64, %rax
	movq	%rax, %rdi
	call	sem_wait@PLT
	movl	-12(%rbp), %edx
	movq	-8(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	put_item
	movq	-8(%rbp), %rax
	addq	$96, %rax
	movq	%rax, %rdi
	call	sem_post@PLT
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -20(%rbp)
.L5:
	movl	-20(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jl	.L6
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	producer, .-producer
	.section	.rodata
.LC1:
	.string	"Skonsumowano: %d\n"
	.text
	.globl	consumer
	.type	consumer, @function
consumer:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	movl	%eax, -16(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L9
.L10:
	movq	-8(%rbp), %rax
	addq	$96, %rax
	movq	%rax, %rdi
	call	sem_wait@PLT
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	get_item
	movl	%eax, -12(%rbp)
	movq	-8(%rbp), %rax
	addq	$64, %rax
	movq	%rax, %rdi
	call	sem_post@PLT
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -20(%rbp)
.L9:
	movl	-20(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jl	.L10
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	consumer, .-consumer
	.section	.rodata
.LC2:
	.string	"Blad ilosci danych"
	.align 8
.LC3:
	.string	"Start: M=%d, N=%d. Bufor o rozmiarze %d.\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$208, %rsp
	movl	%edi, -196(%rbp)
	movq	%rsi, -208(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	cmpl	$3, -196(%rbp)
	je	.L13
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$1, %eax
	jmp	.L15
.L13:
	movq	-208(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, -184(%rbp)
	movq	-208(%rbp), %rax
	addq	$16, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	movl	%eax, -180(%rbp)
	movl	-180(%rbp), %eax
	movl	%eax, -136(%rbp)
	movl	-184(%rbp), %eax
	movl	%eax, -124(%rbp)
	movl	$0, -128(%rbp)
	movl	$0, -132(%rbp)
	movl	-180(%rbp), %eax
	cltq
	salq	$2, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -144(%rbp)
	leaq	-144(%rbp), %rax
	addq	$24, %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_mutex_init@PLT
	movl	-180(%rbp), %eax
	leaq	-144(%rbp), %rdx
	leaq	64(%rdx), %rcx
	movl	%eax, %edx
	movl	$0, %esi
	movq	%rcx, %rdi
	call	sem_init@PLT
	leaq	-144(%rbp), %rax
	addq	$96, %rax
	movl	$0, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	sem_init@PLT
	movl	-180(%rbp), %ecx
	movl	-180(%rbp), %edx
	movl	-184(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	-144(%rbp), %rdx
	leaq	-176(%rbp), %rax
	movq	%rdx, %rcx
	leaq	producer(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create@PLT
	leaq	-144(%rbp), %rax
	leaq	-176(%rbp), %rdx
	leaq	8(%rdx), %rdi
	movq	%rax, %rcx
	leaq	producer(%rip), %rax
	movq	%rax, %rdx
	movl	$0, %esi
	call	pthread_create@PLT
	leaq	-144(%rbp), %rdx
	leaq	-160(%rbp), %rax
	movq	%rdx, %rcx
	leaq	consumer(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create@PLT
	leaq	-144(%rbp), %rax
	leaq	-160(%rbp), %rdx
	leaq	8(%rdx), %rdi
	movq	%rax, %rcx
	leaq	consumer(%rip), %rax
	movq	%rax, %rdx
	movl	$0, %esi
	call	pthread_create@PLT
	movq	-176(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
	movq	-168(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
	movq	-160(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
	movq	-152(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join@PLT
	movq	-144(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	leaq	-144(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rdi
	call	pthread_mutex_destroy@PLT
	leaq	-144(%rbp), %rax
	addq	$64, %rax
	movq	%rax, %rdi
	call	sem_destroy@PLT
	leaq	-144(%rbp), %rax
	addq	$96, %rax
	movq	%rax, %rdi
	call	sem_destroy@PLT
	movl	$0, %eax
.L15:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L16
	call	__stack_chk_fail@PLT
.L16:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
