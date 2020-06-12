#define NB_THREADS 8

	.section	.rodata
	.align	2
hello:
	.string	"Hello "
world:
	.string	"world from thread %d\n"

	.text
	.align	2
	.globl	main
main:
	# Prologue
	addi    sp, sp, -16
	sw      ra, 12(sp)
	sw      s0, 8(sp)
	addi    s0, sp, 16

	addi    s1, x0, NB_THREADS  # Nombre de threads fils à allouer dans s1

# Boucle d'allocation
.L1:
	addi    t1, s1, -1          # Décrément du nombre de threads à allouer
	beq     x0, t1, .L2         # Plus de thread à allouer ?

	                            # Si des threads restent à allouer
	                            # t6 contient l'id du fils
	                            # s1 contient le nombre de threads fils
	                            # s2 contient le couple (père, fils)
	addi    x0, t0, 0
	p_set   t0, t0              # Thread actuel
	p_fn    t6                  # Allocation d'un nouveau thread
	p_swcv  t1, 0(t6)           # Transfère du nombre de threads fils à allouer vers 0(sp)
	p_merge s2, t0, t6          # Création du couple (id père, id fils) dans s2
	p_syncm                     # Synchronisation mémoire
	p_jal   ra, t6, .L3         # Le thread père effectue son travail
	p_lwcv  s1, 0               # Chargement du nombre de threads fils à allouer
	jal     x0, .L1             # Le thread alloué retourne se charger d'allouer ses fils

# Dernier thread seulement
.L2:
	                            # Plus de threads fils, donc ce thread est le dernier
	addi    s2, x0, -1          # Le dernier thread charge le code de retour -1

# Corps de la boucle
.L3:
	# Appel printf(hello)
	lui     a0, %hi(hello)
	addi    a0, a0, %lo(hello)
	call    printf

	# Appel printf(world, i)
	add     a1, s1, x0
	lui     a0, %hi(prt)
	addi    a0, a0, %lo(prt)
	call    printf

	p_jalr  x0, ra, s2           # p_ret : join des threads

.L3:
	# Épilogue
	lui     a0, 0
	lw      ra, 12(sp)
	lw      s0, 8(sp)
	addi    sp, sp, 16
	jalr    x0, 0(ra)
