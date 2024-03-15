;-*-asm-*-
	include "jaghard.inc"
	include "help.mac"

DSP_Audio_frequence     equ 32000
SCLK_PAL        equ ((26593900*4/DSP_Audio_frequence+128)>>8)-1

	dsp

	run	D_RAM
	movei	#init,r0
	jump	(r0)
	nop
	align 16
SP_TOP:

i2s:
	movei  #D_FLAGS,r30
	load   (r30),r29
	bset   #10,r29
	bclr   #3,r29

	addq	#1,r0
	moveta	r0,r10

; return from interrupt I2S
	load   (r31),r1

	addqt  #2,r1
	addqt   #4,r31		; crash
//->	addq	#4,r31		; ok
	jump   (r1)
	store  r29,(R30)

init:
	movei	#SP_TOP,r31
	moveta	r31,r31
	movei  #SCLK_PAL,r11
	movei  #SCLK,r0
	moveq  #%001101,r13     ; SMODE bascule sur RISING
	store  r11,(r0)         ; SCLK
	addq   #4,r0
	store  r13,(r0)         ; SMODE

	moveq	#0,r10
	moveq	#0,r11

	movei  #D_FLAGS,r24
	movei  #D_I2SENA|REGPAGE,r29
	store  r29,(r24)
	nop
	nop

loop:
	movei	#1000,r0
	cmp	r0,r10
	jr	ne,loop
	addq	#1,r11

	STOP_DSP
