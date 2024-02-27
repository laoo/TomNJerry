;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp

	run	D_RAM

    movei    #xx, r10
	  jr    xx
yy: move    pc, r0	//yields yy
zz: nop
xx:
    STOP_DSP
