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
	                         # Instructions renommées       | Table d'association suite à l'exécution de l'instruction
	                         #
	                         # État initial                 | { sp → r0 , ra → r1 , s0 → r2 , a0 → r3 , a1 → r4 , x0 → r5 , x1 → r6 }
							 #
	                         # Prologue
	                         # &i = -12(s0)
	addi    sp, sp, -16      # addi    r7, r0, -16          | { sp → r7 , ra → r1 , s0 → r2 , a0 → r3 , a1 → r4 , x0 → r5 , x1 → r6 }
	sw      ra, 12(sp)       # sw      r8, 12(r7)           | { sp → r7 , ra → r8 , s0 → r2 , a0 → r3 , a1 → r4 , x0 → r5 , x1 → r6 }
	sw      s0, 8(sp)        # sw      r9, 8(r7)            | { sp → r7 , ra → r8 , s0 → r9 , a0 → r3 , a1 → r4 , x0 → r5 , x1 → r6 }
	addi    s0, sp, 16       # addi    r10, r7, 16          | { sp → r7 , ra → r8 , s0 → r10, a0 → r3 , a1 → r4 , x0 → r5 , x1 → r6 }

	                         # Appel scanf(get, &i)
	addi    a1, s0, -12      # addi    r11, r10, -12        | { sp → r7 , ra → r8 , s0 → r10, a0 → r3 , a1 → r11, x0 → r5 , x1 → r6 }
	lui     a0, %hi(get)     # lui     r12, %hi(get)        | { sp → r7 , ra → r8 , s0 → r10, a0 → r12, a1 → r11, x0 → r5 , x1 → r6 }
	addi    a0, a0, %lo(get) # addi    r13, r12, %lo(get)   | { sp → r7 , ra → r8 , s0 → r10, a0 → r13, a1 → r11, x0 → r5 , x1 → r6 }
	call    scanf            # auipc   r14, %hi(scanf)      | { sp → r7 , ra → r8 , s0 → r10, a0 → r13, a1 → r11, x0 → r5 , x1 → r14 }
	                         # jalr    r15, %lo(scanf)(r14) | { sp → r7 , ra → r8 , s0 → r10, a0 → r13, a1 → r11, x0 → r5 , x1 → r15 }
	                         # Le saut génère une nouvelle
	                         # table d'association :        | { sp → r16, ra → r17, s0 → r18, a0 → r19, a1 → r20, x0 → r21, x1 → r22 }

	                         # Appel printf(prt, i)
	lw      a1, -12(s0)      # lw      r23, -12(r18)        | { sp → r16, ra → r17, s0 → r18, a0 → r19, a1 → r23, x0 → r21, x1 → r22 }
	lui     a0, %hi(prt)     # lui     r24, %hi(prt)        | { sp → r16, ra → r17, s0 → r18, a0 → r24, a1 → r23, x0 → r21, x1 → r22 }
	addi    a0, a0, %lo(prt) # addi    r25, r24, %lo(prt)   | { sp → r16, ra → r17, s0 → r18, a0 → r25, a1 → r23, x0 → r21, x1 → r22 }
	call    printf           # auipc   r26, %hi(printf)     | { sp → r16, ra → r17, s0 → r18, a0 → r25, a1 → r23, x0 → r21, x1 → r26 }
	                         # jalr    r27, %lo(printf)(r26)| { sp → r16, ra → r17, s0 → r18, a0 → r25, a1 → r23, x0 → r21, x1 → r27 }
	                         # Le saut génère une nouvelle
	                         # table d'association :        | { sp → r28, ra → r29, s0 → r30, a0 → r31, a1 → r32, x0 → r33, x1 → r34 }

	                         # Épilogue
	lui     a0, 0            # lui     r34, 0               | { sp → r28, ra → r29, s0 → r30, a0 → r34, a1 → r32, x0 → r33, x1 → r34 }
	lw      ra, 12(sp)       # lw      r35, 12(r28)         | { sp → r28, ra → r35, s0 → r30, a0 → r34, a1 → r32, x0 → r33, x1 → r34 }
	lw      s0, 8(sp)        # lw      r36, 8(r28)          | { sp → r28, ra → r35, s0 → r36, a0 → r34, a1 → r32, x0 → r33, x1 → r34 }
	addi    sp, sp, 16       # addi    r37, r28, 16         | { sp → r37, ra → r35, s0 → r36, a0 → r34, a1 → r32, x0 → r33, x1 → r34 }
	jalr    x0, 0(ra)        # jalr    r38, 0(r35)          | { sp → r37, ra → r35, s0 → r36, a0 → r34, a1 → r32, x0 → r38, x1 → r34 }
