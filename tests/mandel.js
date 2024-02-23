;; -*-asm-*-
;; Mandelbrot
;; 224*224*8 Bit and 100 iterations : <1s
;;
;; (c) 1994 Bastian Schick
;;

	dsp

		MACRO BL
		move  pc,LR
		jump  \0
		addqt #6,LR
		ENDM


screen		equ $100000

pu_contrl	equ $f1a114
start		equ $f1b010

genau	equ	13

;;****************************
;;	Regs

i_min		reg	0
r_min		reg	1

delta		reg	3
_i0		reg	4
y_count		reg	5
_r0		reg	6
x_count		reg	7

iter_count	reg	10
_r		reg	11
_i		reg	12
_r1		reg	13
_i1		reg	14
h1		reg	15

ColorIndex	reg	17

max_xy		reg	20	;	window-size
LOOP_X		reg	24
LOOP_Y		reg	25
ITER_LOOP	reg	26
ITER_EXIT	reg	27
txt_ptr		reg	28
screen_ptr	reg	29
ITER_ENDE	reg	30
LR		reg	31

;;	Alternative
MaxIter.a	reg	30

;;*********************

	run	start-$10
	dc.l	$74
	dc.l	$ffffbdb9
	dc.l	$000032e1
	dc.l	100

	nop
	nop
	nop
	nop
	movei	#iter_ende,ITER_ENDE
	movei	#screen,screen_ptr
	moveta	screen_ptr,screen_ptr
	movei	#iter_loop,ITER_LOOP
	movei	#iter_exit,ITER_EXIT
	movei	#loop_y,LOOP_Y
	movei	#loop_x,LOOP_X
	movei	#224,max_xy

//->	movei	#224*224,r17
//->.l	subq	#1,r17
//->	storeb	r17,(screen_ptr)
//->	jr	ne,.l
//->	addq	#1,screen_ptr
//->	movei	#screen,screen_ptr

	movei	#start-16,r14
	load	(r14),delta
	load	(r14+4),r_min
	load	(r14+8),i_min
	load	(r14+12),r10
	moveta	r10,MaxIter.a

;;********************************

	move	i_min,_i0		; i0=i_min
	move	max_xy,y_count		; ycount = 224
	move	r_min,_r0		; r0=r_min

loop_y
	move	max_xy,x_count		; xcount = 224
	movefa	MaxIter.a,iter_count	; max_iter

loop_x
	moveq	#0,ColorIndex
	move	_r0,_r			; r=r0
	move	_i0,_i			; i=i0
	move	_r0,_r1
	move	_i0,_i1

	imult	_r1,_r1			; r^2

iter_loop
	REPT	3
	imult	_i1,_i1			; i^2

	move	_r1,h1
	sub	_i1,_r1			; r1= r^2-i^2
	add	_i1,h1			; h1= r^2+i^2
	sharq	#genau,_r1		; normalize r1

	shrq	#26+2,h1		; >4 ?
	addqt	#1,ColorIndex
	jump	nz,(ITER_ENDE)

	imult	_r,_i
	add	_r0,_r1			; r1=r^2-i^2+r0
	sharq	#genau-1,_i
	move	_r1,_r
	add	_i0,_i			; i=2*i*r+i0

	subq	#1,iter_count
	move	_i,_i1
	jump	z,(ITER_EXIT)
	imult	_r1,_r1
	ENDR

	imult	_i1,_i1			; i^2

	move	_r1,h1
	sub	_i1,_r1			; r1= r^2-i^2
	add	_i1,h1			; h1= r^2+i^2
	sharq	#genau,_r1		; normalize r1

	shrq	#26+2,h1		; >4 ?
	addqt	#1,ColorIndex
	jump	nz,(ITER_ENDE)

	imult	_r,_i
	add	_r0,_r1			; r1=r^2-i^2+r0
	sharq	#genau-1,_i
	move	_r1,_r
	add	_i0,_i			; i=2*i*r+i0

	subq	#1,iter_count
	move	_i,_i1
	jump	nz,(ITER_LOOP)
	imult	_r1,_r1

iter_exit
	moveq	#9,ColorIndex

iter_ende
	add	delta,_r0
	storeb	ColorIndex,(screen_ptr)
	subq	#1,x_count		; xcount -=1
	addqt	#1,screen_ptr
	jump	nz,(LOOP_X)
	movefa	MaxIter.a,iter_count	; max_iter

//->	sub	delta,_i0
//->	add	max_xy,screen_ptr
	subq	#1,y_count
	move	r_min,_r0		; r0=r_min
	jump	nz,(LOOP_Y)
	sub	delta,_i0		; i0+=delta
;;***********************
	movefa	screen_ptr,screen_ptr
	movei	#start-16,r14
	movei	#drawHex,r10

	load	(r14),r0
	BL	(r10)
	addq	#2,screen_ptr
	load	(r14+4),r0
	BL	(r10)
	addq	#2,screen_ptr
	load	(r14+8),r0
	BL	(r10)
	addq	#2,screen_ptr
	load	(r14+12),r0
	BL	(r10)
	addq	#2,screen_ptr


	movei	#pu_contrl,r3
	moveq	#0,r0
wait
	store	r0,(r3)
	jr	wait
	nop
;;***********************

	align	4
drawHex::
	moveq	#8,r5
	move	pc,r6
	subq	#1,r5
	jump	mi,(LR)
	rorq	#28,r0
drawDigit::
	movei	#_0,r1
	moveq	#14,r3
	and	r0,r3
	shlq	#1,r3
	add	r3,r1
	btst	#0,r0
	load	(r1),r1
	jr	eq,.even
	move	screen_ptr,r4
	shlq	#16,r1
.even
	addqt	#4,screen_ptr
	shrq	#16,r1
.ly
	moveq	#3,r2
.lx
	shrq	#1,r1
	jump	eq,(r6)
	subc	r3,r3		; 0 or -1
	subq	#1,r2
	storeb	r3,(r4)
	jr	ne,.lx
	addqt	#1,r4

	movei	#224-3,r2
	moveq	#0,r3
	storeb	r3,(r4)
	jr	.ly
	add	r2,r4

_000	equ %000
_100	equ %001
_010	equ %010
_110	equ %011
_001	equ %100
_101	equ %101
_011	equ %110
_111	equ %111

	align 4
	MACRO char
_\0
	dc.w $8000|(\5<<12)|(\4<<9)|(\3<<6)|(\2<<3)|(\1<<0)
	ENDM

	char 0,_010,_101,_101,_101,_010
	char 1,_010,_110,_010,_010,_010
	char 2,_010,_101,_001,_010,_111
	char 3,_110,_001,_010,_001,_110
	char 4,_100,_101,_111,_001,_001
	char 5,_111,_100,_110,_001,_110
	char 6,_011,_100,_110,_101,_010
	char 7,_111,_001,_001,_010,_010
	char 8,_010,_101,_010,_101,_010
	char 9,_010,_101,_011,_001,_110
	char A,_010,_101,_111,_101,_101
	char B,_110,_101,_110,_101,_110
	char C,_010,_101,_100,_101,_010
	char D,_110,_101,_101,_101,_110
	char E,_111,_100,_110,_100,_111
	char F,_111,_100,_110,_100,_100
drawHex_e:
sizedh	equ drawHex_e-drawHex
	echo "drawHex:%HdrawHex %Dsizedh _0 %H_0"
