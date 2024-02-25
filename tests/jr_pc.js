;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp

	run	D_RAM

	jr	xx
__pc	move	pc,r10
	nop
xx:
	movei	#__pc,r11
	cmp	r10,r11
	moveq	#10,r0
	jr	eq,ok
	nop
	moveq	#5,r0
ok:

	STOP_DSP
