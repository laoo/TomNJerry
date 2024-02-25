;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp
	run D_RAM

	movei	#$80000000,r8
	abs	r8
	movei	#-100,r9
	abs	r9
	movei	#100,r10
	abs	r10

	STOP_DSP
