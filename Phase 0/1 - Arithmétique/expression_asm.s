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

	# a0 <- i
	lw		a0, -12(s0)

	# t1 <- 0x5e301a8b
	lui		t1, 0x5e302 # +1 car le msb de 0xa8b est set
	addi	t1, t1, -1397 # 0xa8b (l'écriture n'est pas supportée ici)

	# t1 <- 0x5e301a8b + i
	add		t1, t1, a0

	# t1 <- t1 <<< 5
	slli	t2, t1, 5
	srli	t1, t1, 27
	or		t1, t1, t2

	# t2 <- 0x3309
	lui		t2, 0x00003
	addi	t2, t2, 0x309

	# t2 <- i & 0x3309
	and		t2, a0, t2

	# a1 <- expression complète
	sub		a1, t1, t2

	# Appel printf(prt, result)
	lui		a0, %hi(prt)
	addi	a0, a0, %lo(prt)
	call	printf

	# Épilogue
	lui		a0, 0
	lw		ra, 12(sp)
	lw		s0, 8(sp)
	addi	sp, sp, 16
	jalr	x0, 0(ra)
