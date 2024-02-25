;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp

	run	D_RAM

	jr	xx
	movei	#$12345678,r10
	nop
xx:
	nop
	nop
	movei	#$e400e400,r11
	cmp	r10,r11

	moveq	#10,r0
	jr	eq,ok
	nop
	moveq	#5,r0
ok:
	STOP_DSP
