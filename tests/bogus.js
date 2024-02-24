    include "jaghard.inc"

    dsp

    run     D_RAM
  

    movei    #D_RAM+100, r13
    moveq    #9, r2
    store    r2, (r13)
    nop
    nop
    movei    #$1234abc, r1
    load     (r13), r0
    move     r1, r0   ; r0 = 00000abc
