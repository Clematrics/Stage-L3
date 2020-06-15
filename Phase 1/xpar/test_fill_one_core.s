main:
                li      t0,-1
                addi    sp,sp,-8
                sw      ra,0(sp)
                sw      t0,4(sp)
                p_set   t0
                jal     ra,init
                lw      ra,0(sp)
                lw      t0,4(sp)
                addi    sp,sp,8
                p_ret
init:
                li      a0,0
.L1:
                p_fc    t6
                p_swcv  ra,0
                p_swcv  t0,4
                p_swcv  a0,8
                p_merge  t0,t0,t6
                p_syncm
                p_jal   ra,init_set
                p_lwcv  ra,0
                p_lwcv  t0,4
                p_lwcv  a0,8
                addi    a0,a0,1
                li      s0,3
                bne     a0,s0,.L1
                p_ret
init_set:
                p_ret


# T0 : ra = 0, t0 = T0, T1 | ligne 30 : se met en attente et attend T1 | ra = 8, saute en 8 | p_ret avec t0 = -1 : quitte le programme
# T1 : ra = 0, t0 = T0, T2 | ligne 30 : attend T2 puis termine
# T2 : ra = 0, t0 = T0, T3 | ligne 30 : attend T3 puis termine
# T3 : ra = 8, t0 = T0     | ligne 25 : envoie ra à T0