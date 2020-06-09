	.section	.rodata
	.align	2
get:
	.string	"%u"
prt:
	.string	"%u\n"

# fib implémente directement fib_internal du fichier .c
	.text
	.align	2
	.globl	fib
fib:
	# Prologue
	addi    sp, sp, -16
	sw      ra, 12(sp)
	sw      s0, 8(sp)
	addi    s0, sp, 16

	beq     a0, x0, .fibL1
	addi    a0, a0, -1
	add     a2, a1, a2
	sub     a1, a2, a1
	call    fib
	jal     x0, .fibL2

.fibL1:
	addi    a0, a1, 0

.fibL2:
	# Épilogue
	lw      ra, 12(sp)
	lw      s0, 8(sp)
	addi    sp, sp, 16
	jalr    x0, 0(ra)

	.text
	.align	2
	.globl	main
main:
	# Prologue
	# &i = -12(s0)
	addi    sp, sp, -16
	sw      ra, 12(sp)
	sw      s0, 8(sp)
	addi    s0, sp, 16

	# Appel scanf(get, &i)
	addi    a1, s0, -12
	lui     a0, %hi(get)
	addi    a0, a0, %lo(get)
	call    scanf

    lw      a0, -12(s0)
    addi    a1, x0, 0
    addi    a2, x0, 1
	call    fib

	# Appel printf(prt, i)
	addi    a1, a0, 0
	lui     a0, %hi(prt)
	addi    a0, a0, %lo(prt)
	call    printf

	# Épilogue
	lui     a0, 0
	lw      ra, 12(sp)
	lw      s0, 8(sp)
	addi    sp, sp, 16
	jalr    x0, 0(ra)
