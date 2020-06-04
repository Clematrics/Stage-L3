	.section	.rodata
	.align	2
get:
	.string	"%d"
negative:
	.string	"< 0\n"
between:
	.string	"0 <= i < 100\n"
big:
	.string	">= 100\n"

	.text
	.align	2
	.globl	main
main:
	# Prologue
	# &i = -12(s0)
	addi	sp, sp, -16
	sw		ra, 12(sp)
	sw		s0, 8(sp)
	addi	s0, sp, 16

	# Appel scanf(get, &i)
	addi	a1, s0, -12
	lui		a0, %hi(get)
	addi	a0, a0, %lo(get)
	call	scanf

	# t1 <- i
	lw		t1, -12(s0)

	# i < 0?
	blt		t1, x0, .L3

	# i < 100 ?
	addi	t2, x0, 100
	blt		t1, t2, .L2

.L1:
	# Appel printf(">= 100\n")
	lui		a0, %hi(big)
	addi	a0, a0, %lo(big)
	call	printf
	jal		x0, .L4

.L2:
	# Appel printf("0 <= i < 100\n")
	lui		a0, %hi(between)
	addi	a0, a0, %lo(between)
	call	printf
	jal		x0, .L4

.L3:
	# Appel printf("< 0\n")
	lui		a0, %hi(negative)
	addi	a0, a0, %lo(negative)
	call	printf

.L4:
	# Épilogue
	lui		a0, 0
	lw		ra, 12(sp)
	lw		s0, 8(sp)
	addi	sp, sp, 16
	jalr	x0, 0(ra)
