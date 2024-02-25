;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp
	run D_RAM

	movei	#$80000000,r8
	sharq	#8,r8

	STOP_DSP
