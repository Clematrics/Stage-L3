# Par simplicité, je suppose que le nombre de threads divise la taille du tableau, i.e. 16384
#define NB_THREADS 8
#define SIZE 16384

	.section	.rodata
	.align	2
result:
	.string	"Result is %u\n"

# Tableau : unsigned int array[16384];
	.text
	.align	2
	.comm	array,(SIZE*4),4
	.align	2
	.comm	sum_thread,(NB_THREADS*4),4

	.text
	.align	2
	.globl	main
main:
	# Prologue / Remplacer par rien si sur FPGA
	addi    sp, sp, -16
	sw      ra, 12(sp)
	sw      s0, 8(sp)

# Remplissage du tableau ########################################

	addi    a0, x0, 0                # Numéro du thread dans a0
	p_set   t0, x0                   # Thread courant dans t0
	lui     ra, %hi(.L6)             # Adresse de retour (suite du main) du premier thread dans ra
	addi    ra, ra, %lo(.L6)         #

# Boucle d'allocation
.L1:
	addi    t1, a0, 1                # Création du numéro du thread fils
	addi    t2, x0, NB_THREADS       # t2 contient le nombre de threads à allouer au total
	beq     t1, t2, .L4              # Plus de thread à allouer ?

	                                 # Si des threads restent à allouer
	                                 # t6 contient l'id du fils
	                                 # a0 contient le numéro du thread fils
	                                 # t0 contient le couple (premier de la pool, fils du thread actuel)
	andi    t2, t1, 0x3              #
	beq     x0, t2, .L2              # Si numéro du thread % 4 = 0, allocation sur nouveau cœur, sinon
	p_fc    t6                       # Allocation d'un nouveau thread sur le même cœur
	jal     x0, .L3                  #
.L2:
	p_fn    t6                       # Allocation d'un nouveau thread sur le cœur suivant
.L3:
	p_swcv  t6, ra, 8                # Transfère de l'adresse de retour
	p_swcv  t6, t0, 4                # Transfère de l'identifiant du premier thread
	p_swcv  t6, t1, 0                # Transfère du numéro du thread fils
	p_merge t0, t0, t6               # Création du couple (premier de la pool, fils du thread actuel) dans t0
	p_syncm                          # Synchronisation mémoire
	p_jal   ra, t0, .L4              # Le thread courant effectue son travail
	p_lwcv  a0, 0                    # Chargement du nombre de threads fils à allouer
	p_lwcv  t0, 4                    # Chargement de l'identifiant du premier thread
	p_lwcv  ra, 8                    # Chargement de l'adresse de retour
	jal     x0, .L1                  # Le thread alloué retourne se charger d'allouer ses fils

# Initialisation de la boucle
.L4:
	lui     a1, %hi(array)       # a1 contient l'adresse de array
	addi    a1, a1, %lo(array)

    addi    t1, x0, SIZE
	addi    t2, x0, NB_THREADS
    divu    t2, t1, t2               # Nombre de cases à traiter dans t2
    mulu    t1, a0, t2               # Indice de départ dans t1
    add     t3, t1, x0               # Nombre à mettre dans la première case dans t3 (incrémenté 1 par 1)
    addi    t2, t1, t2               # Indice d'arrivée dans t2
    slli    t1, t1, 2
    addi    t1, a1, t1               # Adresse de départ dans t1 (sert d'indice de boucle, incrémenté 4 par 4)
    slli    t2, t2, 2
    addi    t2, a1, t2               # Adresse d'arrivée dans t2
    jal     x0, .L6

# Corps de la boucle
.L5:
	sw      t3, 0(t1)
	addi    t3, t3, 1
	addi    t1, t1, 4
.L6:
	bne     t1, t2, .L5

	p_jalr  x0, ra, t0               # Si premier thread : attend les autres thread, récupère ra et saute en ra (.L6)
	                                 # Si autre thread : attend le thread fils puis passe la main à son père
	                                 # Si dernier thread : envoie ra au premier thread et passe la main au père

# Calcul de la somme locale ########################################
.L6:
	addi    a0, x0, 0                # Numéro du thread dans a0
	p_set   t0, x0                   # Thread courant dans t0
	lui     ra, %hi(.L13)            # Adresse de retour (suite du main) du premier thread dans ra
	addi    ra, ra, %lo(.L13)        #

# Boucle d'allocation
.L7:
	addi    t1, a0, 1                # Création du numéro du thread fils
	addi    t2, x0, NB_THREADS       # t2 contient le nombre de threads à allouer au total
	beq     t1, t2, .L10             # Plus de thread à allouer ?

	                                 # Si des threads restent à allouer
	                                 # t6 contient l'id du fils
	                                 # a0 contient le numéro du thread fils
	                                 # t0 contient le couple (premier de la pool, fils du thread actuel)
	andi    t2, t1, 0x3              #
	beq     x0, t2, .L8              # Si numéro du thread % 4 = 0, allocation sur nouveau cœur, sinon
	p_fc    t6                       # Allocation d'un nouveau thread sur le même cœur
	jal     x0, .L9                  #
.L8:
	p_fn    t6                       # Allocation d'un nouveau thread sur le cœur suivant
.L9:
	p_swcv  t6, ra, 8                # Transfère de l'adresse de retour
	p_swcv  t6, t0, 4                # Transfère de l'identifiant du premier thread
	p_swcv  t6, t1, 0                # Transfère du numéro du thread fils
	p_merge t0, t0, t6               # Création du couple (premier de la pool, fils du thread actuel) dans t0
	p_syncm                          # Synchronisation mémoire
	p_jal   ra, t0, .L10             # Le thread courant effectue son travail
	p_lwcv  a0, 0                    # Chargement du nombre de threads fils à allouer
	p_lwcv  t0, 4                    # Chargement de l'identifiant du premier thread
	p_lwcv  ra, 8                    # Chargement de l'adresse de retour
	jal     x0, .L7                  # Le thread alloué retourne se charger d'allouer ses fils

# Initialisation de la boucle
.L10:
	lui     a1, %hi(array)           # a1 contient l'adresse de array
	addi    a1, a1, %lo(array)

    addi    t1, x0, SIZE
	addi    t2, x0, NB_THREADS
    divu    t2, t1, t2               # Nombre de cases à traiter dans t2
    mulu    t1, a0, t2               # Indice de départ dans t1
    addi    t2, t1, t2               # Indice d'arrivée dans t2
    slli    t1, t1, 2
    addi    t1, a1, t1               # Adresse de départ dans t1 (sert d'indice de boucle, incrémenté 4 par 4)
    slli    t2, t2, 2
    addi    t2, a1, t2               # Adresse d'arrivée dans t2
    addi    t3, x0, 0                # Accumulateur de la somme locale dans t3
    jal     x0, .L12

# Corps de la boucle
.L11:
	lw      t4, 0(t1)
	add     t3, t3, t4
	addi    t1, t1, 4
.L12:
	bne     t1, t2, .L11

	lui     a1, %hi(sum_thread)      # a1 contient l'adresse de sum_thread
	addi    a1, a1, %lo(sum_thread)
	slli    t1, a0, 2
	add     a1, a1, t1
	sw      t3, 0(a1)                # Stockage de la somme locale dans le slot mémoire réservé au thread

	p_jalr  x0, ra, t0               # Si premier thread : attend les autres thread, récupère ra et saute en ra (.L6)
	                                 # Si autre thread : attend le thread fils puis passe la main à son père
	                                 # Si dernier thread : envoie ra au premier thread et passe la main au père

# Calcul de la somme totale ########################################
.L13:
	addi    t0, x0, 0                # Accumulateur de la somme locale dans t0
	addi    t1, x0, 0                # Indice de boucle dans t1
	addi    t2, x0, NB_THREADS       # t2 contient le nombre de threads à allouer au total

	lui     t3, %hi(sum_thread)      # t3 contient l'adresse de départ (&sum_thread) et est incrémenté 4 par 4
	addi    t3, t3, %lo(sum_thread)

	jal     x0, .L15

.L14:
	lw      t4, 0(t3)
	add     t0, t0, t4

	addi    t1, t1, 1
	addi    t3, t3, 4
.L15:
	bne     t1, t2, .L14

	# Appel printf(result, sum)
	add     a1, t0, x0
	lui     a0, %hi(result)
	addi    a0, a0, %lo(result)
	call    printf

	# Épilogue / Remplacer par addi t0, x0, -1; p_jalr x0, x0, t0 (exit) si sur FPGA
	lui     a0, 0
	lw      ra, 12(sp)
	addi    sp, sp, 16
	jalr    x0, 0(ra)
