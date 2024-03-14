;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp

	run	D_RAM
	movei	#$1234,r0
	movei	#JOYBUTS,r10
	loadw	(r10),r1
	btst   #4,r1
	jr     eq,initPAL
	nop
	movei	#$5678,r0
initPAL:

	STOP_DSP
