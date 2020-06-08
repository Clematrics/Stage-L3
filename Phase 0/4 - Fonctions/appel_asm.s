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
	addi    sp, sp, -16
	sw      ra, 12(sp)
	sw      s0, 8(sp)
	addi    s0, sp, 16

	# Appel scanf(get, &i)
	addi    a1, s0, -12
	lui     a0, %hi(get)
	addi    a0, a0, %lo(get)
	call    scanf

	# Appel printf(prt, i)
	lw      a1, -12(s0)
	lui     a0, %hi(prt)
	addi    a0, a0, %lo(prt)
	call    printf

	# Épilogue
	lui     a0, 0
	lw      ra, 12(sp)
	lw      s0, 8(sp)
	addi    sp, sp, 16
	jalr    x0, 0(ra)
