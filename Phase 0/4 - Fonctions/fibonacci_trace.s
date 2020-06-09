# Trace de l'appel de fib avec i = 3
                            # Instructions renommées       | Table d'association suite à l'exécution de l'instruction
                            #
                            # État initial                 | { sp → r000, ra → r001, s0 → r002, a0 → r003, a1 → r004, a2 → r005, x0 → r006 }
                            #
lw      a0, -12(s0)         # lw      r007, -12(r002)      | { sp → r000, ra → r001, s0 → r002, a0 → r007, a1 → r004, a2 → r005, x0 → r006 }
addi    a1, x0, 0           # addi    r008, r006, 0        | { sp → r000, ra → r001, s0 → r002, a0 → r007, a1 → r008, a2 → r005, x0 → r006 }
addi    a2, x0, 1           # addi    r009, r006, 1        | { sp → r000, ra → r001, s0 → r002, a0 → r007, a1 → r008, a2 → r009, x0 → r006 }
# call  fib(3, 0, 1)        #                              |
auipc   ra, %hi(fib)        # auipc   r010, %hi(fib)       | { sp → r000, ra → r010, s0 → r002, a0 → r007, a1 → r008, a2 → r009, x0 → r006 }
jalr    ra, %lo(fib)(ra)    # jalr    r011, %lo(fib)(r010) | { sp → r000, ra → r011, s0 → r002, a0 → r007, a1 → r008, a2 → r009, x0 → r006 }
# fib(3, 0, 1)              #                              |
addi    sp, sp, -16         # addi    r012, r000, -16      | { sp → r012, ra → r011, s0 → r002, a0 → r007, a1 → r008, a2 → r009, x0 → r006 }
sw      ra, 12(sp)          # sw      r013, 12(r012)       | { sp → r012, ra → r013, s0 → r002, a0 → r007, a1 → r008, a2 → r009, x0 → r006 }
sw      s0, 8(sp)           # sw      r014, 8(r012)        | { sp → r012, ra → r013, s0 → r014, a0 → r007, a1 → r008, a2 → r009, x0 → r006 }
addi    s0, sp, 16          # addi    r015, r012, 16       | { sp → r012, ra → r013, s0 → r015, a0 → r007, a1 → r008, a2 → r009, x0 → r006 }
                            #                              |
beq     a0, x0, .fibL1      # beq     r016, r006, .fibL1   | { sp → r012, ra → r013, s0 → r015, a0 → r016, a1 → r008, a2 → r009, x0 → r006 }
addi    a0, a0, -1          # addi    r017, r016, -1       | { sp → r012, ra → r013, s0 → r015, a0 → r017, a1 → r008, a2 → r009, x0 → r006 }
add     a2, a1, a2          # add     r018, r008, r009     | { sp → r012, ra → r013, s0 → r015, a0 → r017, a1 → r008, a2 → r018, x0 → r006 }
sub     a1, a2, a1          # sub     r019, r018, r018     | { sp → r012, ra → r013, s0 → r015, a0 → r017, a1 → r019, a2 → r018, x0 → r006 }
# call  fib(2, 1, 1)        #                              |
auipc   ra, %hi(fib)        # auipc   r020, %hi(fib)       | { sp → r012, ra → r020, s0 → r015, a0 → r017, a1 → r019, a2 → r018, x0 → r006 }
jalr    ra, %lo(fib)(ra)    # jalr    r021, %lo(fib)(r020) | { sp → r012, ra → r021, s0 → r015, a0 → r017, a1 → r019, a2 → r018, x0 → r006 }
                            #                              |
# fib(2, 1, 1)              #                              |
addi    sp, sp, -16         # addi    r022, r012, -16      | { sp → r022, ra → r021, s0 → r015, a0 → r017, a1 → r019, a2 → r018, x0 → r006 }
sw      ra, 12(sp)          # sw      r023, 12(r022)       | { sp → r022, ra → r023, s0 → r015, a0 → r017, a1 → r019, a2 → r018, x0 → r006 }
sw      s0, 8(sp)           # sw      r024, 8(r022)        | { sp → r022, ra → r023, s0 → r024, a0 → r017, a1 → r019, a2 → r018, x0 → r006 }
addi    s0, sp, 16          # addi    r025, r022, 16       | { sp → r022, ra → r023, s0 → r025, a0 → r017, a1 → r019, a2 → r018, x0 → r006 }
                            #                              |
beq     a0, x0, .fibL1      # beq     r026, r006, .fibL1   | { sp → r022, ra → r023, s0 → r025, a0 → r026, a1 → r019, a2 → r018, x0 → r006 }
addi    a0, a0, -1          # addi    r027, r026, -1       | { sp → r022, ra → r023, s0 → r025, a0 → r027, a1 → r019, a2 → r018, x0 → r006 }
add     a2, a1, a2          # add     r028, r019, r018     | { sp → r022, ra → r023, s0 → r025, a0 → r027, a1 → r019, a2 → r028, x0 → r006 }
sub     a1, a2, a1          # sub     r029, r028, r019     | { sp → r022, ra → r023, s0 → r025, a0 → r027, a1 → r029, a2 → r028, x0 → r006 }
# call  fib(1, 1, 2)        #                              |
auipc   ra, %hi(fib)        # auipc   r030, %hi(fib)       | { sp → r022, ra → r030, s0 → r025, a0 → r027, a1 → r029, a2 → r028, x0 → r006 }
jalr    ra, %lo(fib)(ra)    # jalr    r031, %lo(fib)(r030) | { sp → r022, ra → r031, s0 → r025, a0 → r027, a1 → r029, a2 → r028, x0 → r006 }
                            #                              |
# fib(1, 1, 2)              #                              |
addi    sp, sp, -16         # addi    r032, r022, -16      | { sp → r032, ra → r031, s0 → r025, a0 → r027, a1 → r029, a2 → r028, x0 → r006 }
sw      ra, 12(sp)          # sw      r033, 12(r032)       | { sp → r032, ra → r033, s0 → r025, a0 → r027, a1 → r029, a2 → r028, x0 → r006 }
sw      s0, 8(sp)           # sw      r034, 8(r032)        | { sp → r032, ra → r033, s0 → r034, a0 → r027, a1 → r029, a2 → r028, x0 → r006 }
addi    s0, sp, 16          # addi    r035, r032, 16       | { sp → r032, ra → r033, s0 → r035, a0 → r027, a1 → r029, a2 → r028, x0 → r006 }
                            #                              |
beq     a0, x0, .fibL1      # beq     r036, r006, .fibL1   | { sp → r032, ra → r033, s0 → r035, a0 → r036, a1 → r029, a2 → r028, x0 → r006 }
addi    a0, a0, -1          # addi    r037, r036, -1       | { sp → r032, ra → r033, s0 → r035, a0 → r037, a1 → r029, a2 → r028, x0 → r006 }
add     a2, a1, a2          # add     r038, r029, r028     | { sp → r032, ra → r033, s0 → r035, a0 → r037, a1 → r029, a2 → r038, x0 → r006 }
sub     a1, a2, a1          # sub     r039, r038, r029     | { sp → r032, ra → r033, s0 → r035, a0 → r037, a1 → r039, a2 → r038, x0 → r006 }
# call  fib(0, 2, 3)        #                              |
auipc   ra, %hi(fib)        # auipc   r040, %hi(fib)       | { sp → r032, ra → r040, s0 → r035, a0 → r037, a1 → r039, a2 → r038, x0 → r006 }
jalr    ra, %lo(fib)(ra)    # jalr    r041, %lo(fib)(r040) | { sp → r032, ra → r041, s0 → r035, a0 → r037, a1 → r039, a2 → r038, x0 → r006 }
                            #                              |
# fib(0, 2, 3)              #                              |
addi    sp, sp, -16         # addi    r042, r032, -16      | { sp → r042, ra → r041, s0 → r035, a0 → r037, a1 → r039, a2 → r038, x0 → r006 }
sw      ra, 12(sp)          # sw      r043, 12(r042)       | { sp → r042, ra → r043, s0 → r035, a0 → r037, a1 → r039, a2 → r038, x0 → r006 }
sw      s0, 8(sp)           # sw      r044, 8(r042)        | { sp → r042, ra → r043, s0 → r044, a0 → r037, a1 → r039, a2 → r038, x0 → r006 }
addi    s0, sp, 16          # addi    r045, r042, 16       | { sp → r042, ra → r043, s0 → r045, a0 → r037, a1 → r039, a2 → r038, x0 → r006 }
                            #                              |
beq     a0, x0, .fibL1      # beq     r046, r006, .fibL1   | { sp → r042, ra → r043, s0 → r045, a0 → r046, a1 → r039, a2 → r038, x0 → r006 }
                            #                              |
addi    a0, a1, 0           # addi    r047, r039, 0        | { sp → r042, ra → r043, s0 → r045, a0 → r047, a1 → r039, a2 → r038, x0 → r006 }
                            #                              |
# fib(0, 2, 3) épilogue     #                              |
lw      ra, 12(sp)          # lw      r048, 12(r042)       | { sp → r042, ra → r048, s0 → r045, a0 → r047, a1 → r039, a2 → r038, x0 → r006 }
lw      s0, 8(sp)           # lw      r049, 8(r042)        | { sp → r042, ra → r048, s0 → r049, a0 → r047, a1 → r039, a2 → r038, x0 → r006 }
addi    sp, sp, 16          # addi    r050, r042, 16       | { sp → r050, ra → r048, s0 → r049, a0 → r047, a1 → r039, a2 → r038, x0 → r006 }
jalr    x0, 0(ra)           # jalr    r051, 0(r048)        | { sp → r050, ra → r048, s0 → r049, a0 → r047, a1 → r039, a2 → r038, x0 → r051 }
                            #                              |
jal     x0, .fibL2          # jal     r052, .fibL2         | { sp → r050, ra → r048, s0 → r049, a0 → r047, a1 → r039, a2 → r038, x0 → r052 }
                            #                              |
# fib(1, 1, 2) épilogue     #                              |
lw      ra, 12(sp)          # lw      r053, 12(r050)       | { sp → r050, ra → r053, s0 → r049, a0 → r047, a1 → r039, a2 → r038, x0 → r052 }
lw      s0, 8(sp)           # lw      r054, 8(r050)        | { sp → r050, ra → r053, s0 → r054, a0 → r047, a1 → r039, a2 → r038, x0 → r052 }
addi    sp, sp, 16          # addi    r055, r050, 16       | { sp → r055, ra → r053, s0 → r054, a0 → r047, a1 → r039, a2 → r038, x0 → r052 }
jalr    x0, 0(ra)           # jalr    r056, 0(r053)        | { sp → r055, ra → r053, s0 → r054, a0 → r047, a1 → r039, a2 → r038, x0 → r056 }
                            #                              |
jal     x0, .fibL2          # jal     r057, .fibL2         | { sp → r055, ra → r053, s0 → r054, a0 → r047, a1 → r039, a2 → r038, x0 → r057 }
                            #                              |
# fib(2, 1, 1) épilogue     #                              |
lw      ra, 12(sp)          # lw      r058, 12(r055)       | { sp → r055, ra → r058, s0 → r054, a0 → r047, a1 → r039, a2 → r038, x0 → r057 }
lw      s0, 8(sp)           # lw      r059, 8(r055)        | { sp → r055, ra → r058, s0 → r059, a0 → r047, a1 → r039, a2 → r038, x0 → r057 }
addi    sp, sp, 16          # addi    r060, r055, 16       | { sp → r060, ra → r058, s0 → r059, a0 → r047, a1 → r039, a2 → r038, x0 → r057 }
jalr    x0, 0(ra)           # jalr    r061, 0(r058)        | { sp → r060, ra → r058, s0 → r059, a0 → r047, a1 → r039, a2 → r038, x0 → r061 }
                            #                              |
jal     x0, .fibL2          # jal     r062, .fibL2         | { sp → r060, ra → r058, s0 → r059, a0 → r047, a1 → r039, a2 → r038, x0 → r062 }
                            #                              |
# fib(3, 0, 1) épilogue     #                              |
lw      ra, 12(sp)          # lw      r063, 12(r060)       | { sp → r060, ra → r063, s0 → r059, a0 → r047, a1 → r039, a2 → r038, x0 → r062 }
lw      s0, 8(sp)           # lw      r064, 8(r060)        | { sp → r060, ra → r063, s0 → r064, a0 → r047, a1 → r039, a2 → r038, x0 → r062 }
addi    sp, sp, 16          # addi    r065, r060, 16       | { sp → r065, ra → r063, s0 → r064, a0 → r047, a1 → r039, a2 → r038, x0 → r062 }
jalr    x0, 0(ra)           # jalr    r066, 0(r063)        | { sp → r065, ra → r063, s0 → r064, a0 → r047, a1 → r039, a2 → r038, x0 → r066 }