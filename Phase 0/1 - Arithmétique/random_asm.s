	.section	.rodata
	.align	2
get:
	.string	"%d"
prt:
	.string	"%d\n"

	.text
	.align	2
	.globl	main
main:
	# Prologue
	addi	sp, sp, -16
	sw		ra, 12(sp)
	sw		s0, 8(sp)
	addi	s0, sp, 16

	# s1 <- 0x41C64E6D : multiplier
	lui		s1, 0x41C65 # +1
	addi	s1, s1, -403 # 0xE6D
	# s2 <- 0x2039 : increment
	lui		s2, 0x00002
	addi	s2, s2, 0x039
	# s3 <- 10
	addi	s3, x0, 0x00a
	# s4 <- 0x533DC0DE : seed (i)
	lui		s4, 0x533DC
	addi	s4, s4, 0x0DE

	# s4 <- multiplier * i + increment
	mul		s4, s4, s1
	add		s4, s4, s2
	# a1 <- s4 % 10
	rem		a1, s4, s3
	# Appel printf(prt, i % 10)
	lui		a0, %hi(prt)
	addi	a0, a0, %lo(prt)
	call	printf

	# s4 <- multiplier * i + increment
	mul		s4, s4, s1
	add		s4, s4, s2
	# a1 <- s4 % 10
	rem		a1, s4, s3
	# Appel printf(prt, i % 10)
	lui		a0, %hi(prt)
	addi	a0, a0, %lo(prt)
	call	printf

	# s4 <- multiplier * i + increment
	mul		s4, s4, s1
	add		s4, s4, s2
	# a1 <- s4 % 10
	rem		a1, s4, s3
	# Appel printf(prt, i % 10)
	lui		a0, %hi(prt)
	addi	a0, a0, %lo(prt)
	call	printf

	# Épilogue
	addi	a0, x0, 0
	lw		ra, 12(sp)
	lw		s0, 8(sp)
	addi	sp, sp, 16
	jalr	x0, 0(ra)
