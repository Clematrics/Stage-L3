#define NB_THREADS 8
#define LOCAL_STACK_SIZE 4096

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
	# Prologue / Remplacer par rien si sur FPGA
	addi    sp, sp, -32
	sw      ra, 28(sp)
	sw      s0, 24(sp)
	sw      s1, 20(sp)
	sw      s2, 16(sp)
	sw      s3, 12(sp)

	addi    s1, x0, 0                # Numéro du thread dans s1
	p_set   s2, x0                   # Thread courant dans s2
	lui     s3, %hi(.L5)             # Adresse de retour (épilogue du main) du premier thread dans s3
	addi    s3, s3, %lo(.L5)         #

# Boucle d'allocation
.L1:
	addi    t1, s1, 1                # Création du numéro du thread fils
	addi    t2, x0, NB_THREADS
	beq     t1, t2, .L4              # Plus de thread à allouer ?

	                                 # Si des threads restent à allouer
	                                 # t6 contient l'id du fils
	                                 # t1 contient le numéro du thread fils
	                                 # s2 contient le couple (premier de la pool, fils du thread actuel)
	andi    t2, t1, 0x3              #
	beq     x0, t2, .L2              # Si numéro du thread fils % 4 = 0, allocation sur nouveau cœur, sinon
	p_fc    t6                       # Allocation d'un nouveau thread sur le même cœur
	jal     x0, .L3                  #
.L2:
	p_fn    t6                       # Allocation d'un nouveau thread sur le cœur suivant
.L3:
	addi    t2, sp, LOCAL_STACK_SIZE # Création du stack local pour le hart suivant
	p_swcv  t6, t2, 12               # Transfère du pointeur de pile local au hart suivant
	p_swcv  t6, s3, 8                # Transfère de l'adresse de retour
	p_swcv  t6, s2, 4                # Transfère de l'identifiant du premier thread
	p_swcv  t6, t1, 0                # Transfère du numéro du thread fils
	p_merge s2, s2, t6               # Création du couple (premier de la pool, fils du thread actuel) dans s2
	p_syncm                          # Synchronisation mémoire
	p_jal   s3, s2, .L4              # Le thread courant effectue son travail
	p_lwcv  s1, 0                    # Chargement du nombre de threads fils à allouer
	p_lwcv  s2, 4                    # Chargement de l'identifiant du premier thread
	p_lwcv  s3, 8                    # Chargement de l'adresse de retour
	p_lwcv  sp, 12                   # Chargement du pointeur de pile
	jal     x0, .L1                  # Le thread alloué retourne se charger d'allouer ses fils

# Corps de la boucle
.L4:
	# Appel printf(hello)
	lui     a0, %hi(hello)
	addi    a0, a0, %lo(hello)
	call    printf

	# Appel printf(world, i)
	add     a1, s1, x0
	lui     a0, %hi(prt)
	addi    a0, a0, %lo(prt)
	call    printf

	p_jalr  x0, s3, s2               # Si premier thread : attend les autres thread, récupère s3 et saute en s3 (.L5)
	                                 # Si autre thread : attend le thread fils puis passe la main à son père
	                                 # Si dernier thread : envoie s3 au premier thread et passe la main au père

.L5:
	# Épilogue / Remplacer par addi t0, x0, -1; p_jalr x0, x0, t0 (exit) si sur FPGA
	lui     a0, 0
	lw      s3, 12(sp)
	lw      s2, 16(sp)
	lw      s1, 20(sp)
	lw      s0, 24(sp)
	lw      ra, 28(sp)
	addi    sp, sp, 32
	jalr    x0, 0(ra)
