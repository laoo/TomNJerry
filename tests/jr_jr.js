;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp

	run	D_RAM

	jr	xx
	jr	yy
	nop
xx:
	moveq	#5,r0
	jr	done
	nop
yy:	moveq	#10,r0
	nop
done:

	STOP_DSP
