; -* -asm -* -
  include "jaghard.inc"
	include "help.mac"

    dsp
    run D_RAM

    movei  #D_FLAGS, r11
    movei  #1 << 14, r12
    store  r12, (r11)
    movei    #$12345678, r10
    moveta    r10, r0

    STOP_DSP
