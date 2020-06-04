	.section	.rodata
	.align	2
get:
	.string	"%d"
one:
	.string	"1"
zer:
	.string	"0"
newline:
	.string	"\n"
space:
	.string	" "

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

	# i sera stocké dans s1
	# r sera stocké dans s2
	# m sera stocké dans s3
	# j sera stocké dans s4

	# s1 <- i
	lw		s1, -12(s0)

#### Affichage de i #####

	# s3 <- masque
	lui		s3, 0x80000
	# boucle for, j <- 0 (la première partie peut être sauté, car on peut effectuer un shift logique directement)
	# pas d'unrolling exprès
	lui		s4, 0
	jal		x0, .L2

.L1: # corps
	# Appel printf(i & m ? "1" : "0")
	and		t1, s1, s3
	beq		t1, x0, .L1A
	lui		a0, %hi(one)
	addi	a0, a0, %lo(one)
	jal		x0, .L1B
.L1A:
	lui		a0, %hi(zer)
	addi	a0, a0, %lo(zer)
.L1B:
	call	printf

	# Appel conditionnel printf(" ");
	addi	t1, s4, 1
	addi	t2, x0, 4
	remu	t1, t1, t2
	bne		t1, x0, .L1C

	lui		a0, %hi(space)
	addi	a0, a0, %lo(space)
	call	printf
.L1C:

	# masque >>= 1
	srli	s3, s3, 1
	# incrément de j
	addi	s4, s4, 1
.L2: # test
	addi	t1, x0, 32
	bltu	s4, t1, .L1

	lui		a0, %hi(newline)
	addi	a0, a0, %lo(newline)
	call	printf



#### Calcul du miroir #####
	# r = i
	add		s2, s1, x0
	# s stocké dans s4 (n'interfère pas avec j)
	addi	s4, x0, 31

	# i >>= 1 initial
	srli	s1, s1, 1
	jal		x0, .L4

.L3: # corps
	# r <<= 1
	slli	s2, s2, 1

	# r |= i & 1
	andi	t1, s1, 1
	or		s2, s2, t1

	# décrément de s
	addi	s4, s4, -1
	# i >>= 1
	srli	s1, s1, 1
.L4: # test
	bne		s1, x0, .L3

	# dernier décalage de r‭
	sll		s2, s2, s4



#### Affichage de r #####

	# s3 <- masque
	lui		s3, 0x80000
	# boucle for, j <- 0 (la première partie peut être sauté, comme précédemment)
	lui		s4, 0
	jal		x0, .L6

.L5: # corps
	# Appel printf(r & m ? "1" : "0")
	and		t1, s2, s3
	beq		t1, x0, .L5A
	lui		a0, %hi(one)
	addi	a0, a0, %lo(one)
	jal		x0, .L5B
.L5A:
	lui		a0, %hi(zer)
	addi	a0, a0, %lo(zer)
.L5B:
	call	printf

	# Appel conditionnel printf(" ");
	addi	t1, s4, 1
	addi	t2, x0, 4
	remu	t1, t1, t2
	bne		t1, x0, .L5C

	lui		a0, %hi(space)
	addi	a0, a0, %lo(space)
	call	printf
.L5C:

	# masque >>= 1
	srli	s3, s3, 1
	# incrément de j
	addi	s4, s4, 1
.L6: # test
	addi	t1, x0, 32
	bltu	s4, t1, .L5

	lui		a0, %hi(newline)
	addi	a0, a0, %lo(newline)
	call	printf

	# Épilogue
	lui		a0, 0
	lw		ra, 12(sp)
	lw		s0, 8(sp)
	addi	sp, sp, 16
	jalr	x0, 0(ra)
