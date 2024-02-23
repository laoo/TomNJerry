    include "jaghard.inc"
    
    dsp

rI2SL       reg 99
rI2SR       reg 99
rInd        reg 99
rSamp       reg 99
rI2sAdr     reg 99
rFlagsAdr   reg 99
rFlags      reg 99
rStack      reg 31
rRetAdr     reg 30

    run     D_RAM

    movei   #init,r0
    jump    (r0)
    nop

    org     D_RAM+$10

    jump    (rI2sAdr)
    load    (rFlagsAdr),rFlags

init:
    movei   #D_ENDRAM,r31
    movei   #L_I2S,rI2SL
    movei   #R_I2S,rI2SR
    movei   #ROM_SINE,rInd
    movei   #I2S,rI2sAdr
    movei   #D_MOD,r0
    movei   #MODMASK512,r1
    store   r1,(r0)
    movei   #16,r2
    movei   #SSI_RISING|SSI_WSEN|SSI_INTERNAL,r3
    movei   #SCLK,r0
    movei   #SMODE,r1
    store   r2,(r0)
    store   r3,(r1)
    movei   #D_FLAGS,rFlagsAdr
    movei   #D_I2SENA,r1
    moveq   #0,rSamp
    store   r1,(rFlagsAdr)

    movei   #100,r0
    add     rInd,r0

loop
    cmp     r0, rInd
    jr      cc,loop
    nop
    movei   #D_CTRL, r0
    moveq   #0, r1
    store   r1,(r0)
wait
    jr      wait
    nop

I2S:
    store   rSamp,(rI2SL)
    store   rSamp,(rI2SR)

    load    (rInd),rSamp
    addqmod #4,rInd

    load    (rStack),rRetAdr    ; return address
    bset    #10,rFlags          ; clear latch 1 = I2S
    addqt   #2,rRetAdr          ; next instruction
    bclr    #3,rFlags           ; clear IMASK
    addq    #4,rStack           ; pop from stack
    jump    (rRetAdr)           ; return
    store   rFlags,(rFlagsAdr)  ; restore flags
