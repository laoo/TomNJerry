    include "jaghard.inc"

    dsp

    run     D_RAM
  

  movei    #$1000, r14
    movei    #D_RAM+100, r13
    movei    #$12345678, r1
    store    r1, (r14 + 4)
    moveq    #9, r2
    storeb    r2, (r13)
    div    r2, r1
    or    r1, r1
    jr    eq, test
  load(r14 + 4), r0
  loadb(r13), r0      ;should end first and be overwritten by previous load
test:
    or    r0, r0
