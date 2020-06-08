	.section	.rodata
	.align	2
get:
	.string	"%u"
	.align	2
print0:
	.string	"Fib(%u) : "
	.align	2
print1:
	.string	"%u\n"

# Variables globales
	.text
	.align	2
	.comm	fib_array,256,4
	.section .sbss, "aw", @nobits
	.align	2
fib_index:
	.zero	4

	.text
	.globl  fib_index
	.globl	fib
	.globl	main
	.align	2
fib:
	# Prologue
	# a0 = n
	addi    sp, sp, -16
	sw      ra, 12(sp)
	sw      s0, 8(sp)
	addi    s0, sp, 16

	# a1 = fib_array
	lui     a1, %hi(fib_array)
	addi    a1, a1, %lo(fib_array)

	# a2 = fib_index
	lui     a2, %hi(fib_index)
	lw      a2, %lo(fib_index)(a2)

	bltu    a2, a0, .fibL1     # if (n <= fib_index)

	slli    t1, a0, 2          # return fib_array[n];
	add     t1, a1, t1
	lw      a0, 0(t1)
	jal     x0, .fibL5

.fibL1:
	# a3 = a
	# a4 = b
	slli    t1, a2, 2          # unsigned int a = fib_array[fib_index - 1];
	add     t1, a1, t1
	lw      a3, -4(t1)

	lw      a4, 0(t1)          # unsigned int b = fib_array[fib_index];

	# a5 = i
	addi    a5, a2, 0          # for (unsigned int i = fib_index; __________) {

	jal     x0, .fibL3

.fibL2:
	addi    t1, a5, 1          # fib_array[i + 1] = a + b;
	slli    t1, t1, 2
	add     t1, a1, t1
	add     t2, a3, a4
	sw      t2, 0(t1)

	addi    a3, a4, 0          # a = b;

	addi    t1, a5, 1          # b = fib_array[i + 1];
	slli    t1, t1, 2
	add     t1, a1, t1
	lw      a4, 0(t1)

	addi    a5, a5, 1          # for (_________________________________; i++) {
.fibL3:
	bltu    a5, a0, .fibL2     # for (__________________________; i < n; ____) {

.fibL4:
	lui     t1, %hi(fib_index) # fib_index = n;
	lw      a0, %lo(fib_index)(t1)

	addi    a0, a4, 0          # return b
.fibL5:
	lw      ra, 12(sp)
	lw      s0, 8(sp)
	addi    sp, sp, 16
	jalr    x0, 0(ra)


	.align	2
main:
	# Prologue
	addi    sp, sp, -16
	sw      ra, 12(sp)
	sw      s0, 8(sp)
	addi    s0, sp, 16

	# Initialisation de fib_array[0] et fib_array[1]
	lui     t1, %hi(fib_array)
	addi    t1, t1,%lo(fib_array)
	sw      x0, 0(t1)
	addi    t2, x0, 1
	sw      t2, 4(t1)

	# Initialisation de fib_index
	lui     t1, %hi(fib_index)
	addi    t2, x0, 1
	sw      t2, %lo(fib_index)(t1)

	# Appel i = fib(10)
	addi    a0, x0, 10
	call    fib

	# Appel printf(print1, i)
	addi    a1, a0, 0
	lui     a0, %hi(print1)
	addi    a0, a0, %lo(print1)
	call    printf

	# Appel i = fib(5)
	addi    a0, x0, 5
	call    fib

	# Appel printf(print1, i)
	addi    a1, a0, 0
	lui     a0, %hi(print1)
	addi    a0, a0, %lo(print1)
	call    printf

	# Appel i = fib(5)
	addi    a0, x0, 5
	call    fib

	# Appel printf(print1, i)
	addi    a1, a0, 0
	lui     a0, %hi(print1)
	addi    a0, a0, %lo(print1)
	call    printf

	# Appel i = fib(20)
	addi    a0, x0, 20
	call    fib

	# Appel printf(print1, i)
	addi    a1, a0, 0
	lui     a0, %hi(print1)
	addi    a0, a0, %lo(print1)
	call    printf

	# Appel i = fib(10)
	addi    a0, x0, 10
	call    fib

	# Appel printf(print1, i)
	addi    a1, a0, 0
	lui     a0, %hi(print1)
	addi    a0, a0, %lo(print1)
	call    printf

	# Épilogue
	lui     a0, 0
	lw      ra, 12(sp)
	lw      s0, 8(sp)
	addi    sp, sp, 16
	jalr    x0, 0(ra)
