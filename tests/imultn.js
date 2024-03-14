;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp

	run	D_RAM
	movei	#$1234,r0
	movei	#$5678,r1
	movei	#$abcd,r2
	movei	#$ef01,r3
	moveq	#2,r4
	move	pc,r5
	addq	#4,r5

	imultn	r0,r1
	imacn	r2,r3
	resmac	r0
	sharq	#7,r0
	subq	#1,r4
	jump	ne,(r5)
	nop


	STOP_DSP
