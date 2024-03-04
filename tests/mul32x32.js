;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

	dsp

	run	D_RAM
	movei	#$189ab,r0
	movei	#$189ab,r1
	;; =>	2'5D5E'7839
	nop
	nop
	nop

	move	r1,r2
	rorq	#16,r1
	mult	r0,r2		; r2 = r0.l*r1.l
	imultn	r0,r1		; acc = r0.l*r1.h
	shrq	#16,r0
	rorq	#16,r1
	imacn	r0,r1		; acc += r0.h*r1.l
	resmac	r0
	shlq	#16,r0
	add	r2,r0

	or	r0,r0

	STOP_DSP
