;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;       Hardware equates for JAGUAR System
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;      GENERIC DEFINES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DRAM            equ   $000000           ; First system RAM location
USERRAM         equ   $004000           ; Beginning of non-reserved RAM
ENDRAM          equ   $200000           ; End of DRAM
INITSTACK       equ   ENDRAM-4          ; Good place to initialize stack

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;      CPU REGISTERS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LEVEL0          equ   $100              ; 68000 Level 0 Autovector Interrupt
USER0           equ   $100              ; Pseudonym for LEVEL0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Masks for INT1 CPU Interrupt Control Register

C_VIDENA        equ   $0001             ; Enable Video time-base interrupts
C_GPUENA        equ   $0002             ; Enable GPU register-write interrupt
C_OPENA         equ   $0004             ; Enable OP stop-object interrupt
C_PITENA        equ   $0008             ; Enable PIT interrupt
C_JERENA        equ   $0010             ; Enable Jerry interrupt

C_VIDCLR        equ   $0100             ; Clear pending video interrupts
C_GPUCLR        equ   $0200             ; Clear pending GPU interrupts
C_OPCLR         equ   $0400             ; Clear pending OP interrupts
C_PITCLR        equ   $0800             ; Clear pending PIT interrupts
C_JERCLR        equ   $1000             ; Clear pending Jerry interrupts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       JAGUAR REGISTERS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_BASE           equ   $F00000           ; TOM Internal Register Base

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       TOM REGISTERS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

MEMCON1         equ   $F00000           ; Memory Configuration Register One
MEMCON2         equ   $F00002           ; Memory Configuration Register Two
HC              equ   $F00004           ; Horizontal Count
VC              equ   $F00006           ; Vertical Count
LPH             equ   $F00008           ; Horizontal Lightpen
LPV             equ   $F0000A           ; Vertical Lightpen
OB0             equ   $F00010           ; Current Object Phrase
OB1             equ   $F00012
OB2             equ   $F00014
OB3             equ   $F00016
OLP             equ   $F00020           ; Object List Pointer
ODP             equ   $F00024           ; Object Data Pointer
OBF             equ   $F00026           ; Object Processor Flag
VMODE           equ   $F00028           ; Video Mode
BORD1           equ   $F0002A           ; Border Color (Red & Green)
BORD2           equ   $F0002C           ; Border Color (Blue)
HP              equ   $F0002e           ; Horizontal Period
HBB             equ   $F00030           ; Horizontal Blanking Begin
HBE             equ   $F00032           ; Horizontal Blanking End
HS              equ   $F00034           ; Horizontal Sync
HVS             equ   $F00036           ; Horizontal Vertical Sync
HDB1            equ   $F00038           ; Horizontal Display Begin One
HDB2            equ   $F0003A           ; Horizontal Display Begin Two
HDE             equ   $F0003C           ; Horizontal Display End
VP              equ   $F0003e           ; Vertical Period
VBB             equ   $F00040           ; Vertical Blanking Begin
VBE             equ   $F00042           ; Vertical Blanking End
VS              equ   $F00044           ; Vertical Sync
VDB             equ   $F00046           ; Vertical Display Begin
VDE             equ   $F00048           ; Vertical Display End
VEB             equ   $F0004a           ; Vertical equalization Begin
VEE             equ   $F0004c           ; Vertical equalization End
VI              equ   $F0004E           ; Vertical Interrupt
PIT0            equ   $F00050           ; Programmable Interrupt Timer (Lo)
PIT1            equ   $F00052           ; Programmable Interrupt Timer (Hi)
HEQ             equ   $F00054           ; Horizontal equalization End
BWNOCRY         equ   $F00056           ; "If you set bit 2 in the undocumented register F00056, you get "black and white CRY" mode.
BG              equ   $F00058           ; Background Color

INT1            equ   $F000E0           ; CPU Interrupt Control Register
INT2            equ   $F000E2           ; CPU Interrupt Resume Register

CLUT            equ   $F00400           ; Color Lookup Table

LBUFA           equ   $F00800           ; Line Buffer A
LBUFB           equ   $F01000           ; Line Buffer B
LBUFC           equ   $F01800           ; Line Buffer Current

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;      OBJECT PROCESSOR EQUATES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BITOBJ          equ   0
SCBITOBJ        equ   1
GPUOBJ          equ   2
BRANCHOBJ       equ   3
STOPOBJ         equ   4

O_REFLECT       equ   $00002000         ; OR with top LONG of BITMAP object
O_RMW           equ   $00004000
O_TRANS         equ   $00008000
O_RELEASE       equ   $00010000

O_DEPTH1        equ   (0<<12)           ; DEPTH Field for BITMAP objects
O_DEPTH2        equ   (1<<12)
O_DEPTH4        equ   (2<<12)
O_DEPTH8        equ   (3<<12)
O_DEPTH16       equ   (4<<12)
O_DEPTH32       equ   (5<<12)

O_NOGAP         equ   (1<<15)           ; Phrase GAP between image phrases
O_1GAP          equ   (2<<15)
O_2GAP          equ   (3<<15)
O_3GAP          equ   (4<<15)
O_4GAP          equ   (5<<15)
O_5GAP          equ   (6<<15)
O_6GAP          equ   (7<<15)

O_BREQ          equ   (0<<14)           ; CC field of BRANCH objects
O_BRGT          equ   (1<<14)
O_BRLT          equ   (2<<14)
O_BROP          equ   (3<<14)
O_BRHALF        equ   (4<<14)

O_STOPINTS      equ   $00000008         ; Enable Interrupts in STOP object

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;      VIDEO INITIALIZATION CONSTANTS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

NTSC_WIDTH      equ   1409              ; Width of screen in pixel clocks
NTSC_HMID       equ   823               ; Middle of screen in pixel clocks
NTSC_HEIGHT     equ   241               ; Height of screen in scanlines
NTSC_VMID       equ   266               ; Middle of screen in halflines

PAL_WIDTH       equ   1381              ; Same as above for PAL...
PAL_HMID        equ   843
PAL_HEIGHT      equ   287
PAL_VMID        equ   322

;;; The following mask will extract the PAL/NTSC flag bit from the
;;; CONFIG register. NTSC = Bit Set, PAL = Bit Clear

VIDTYPE         equ   $10
VIDTYPEBIT      equ   4

;;; The following are Video Mode register flags

VIDEN           equ   $0001             ; Enable video-time base generator

CRY16           equ   $0000             ; Enable 16-bit CRY mode
RGB24           equ   $0002             ; Enable 24-bit RGB mode
DIRECT16        equ   $0004             ; Enable 16-bit DIRECT mode
RGB16           equ   $0006             ; Enable 16-bit RGB mode

GENLOCK         equ   $0008             ; Not Supported in Jaguar Console
INCEN           equ   $0010             ; Enable encrustation
BINC            equ   $0020             ; Select local border color
CSYNC           equ   $0040             ; Enable composite sync
BGEN            equ   $0080             ; Clear line buffer to BG
VARMOD          equ   $0100             ; Enable variable-color resolution mode

PWIDTH1         equ   $0000             ; Select pixels 1 clock wide
PWIDTH2         equ   $0200             ; Select pixels 2 clocks wide
PWIDTH3         equ   $0400             ; Select pixels 3 clocks wide
PWIDTH4         equ   $0600             ; Select pixels 4 clocks wide
PWIDTH5         equ   $0800             ; Select pixels 5 clocks wide
PWIDTH6         equ   $0A00             ; Select pixels 6 clocks wide
PWIDTH7         equ   $0C00             ; Select pixels 7 clocks wide
PWIDTH8         equ   $0E00             ; Select pixels 8 clocks wide

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       GPU REGISTERS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

G_FLAGS         equ   $F02100           ; GPU Flags
G_MTXC          equ   $F02104           ; GPU Matrix Control
G_MTXA          equ   $F02108           ; GPU Matrix Address
G_END           equ   $F0210C           ; GPU Data Organization
G_PC            equ   $F02110           ; GPU Program Counter
G_CTRL          equ   $F02114           ; GPU Operation Control/Status
G_HIDATA        equ   $F02118           ; GPU Bus Interface high data
G_REMAIN        equ   $F0211C           ; GPU Division Remainder
G_DIVCTRL       equ   $F0211C           ; GPU Divider control
G_RAM           equ   $F03000           ; GPU Internal RAM
G_ENDRAM        equ   G_RAM+(4*1024)    ; 4K bytes

;;;
;;; GPU Flags Register equates
;;;

G_CPUENA        equ   $00000010         ; CPU Interrupt enable bits
G_DSPENA        equ   $00000020         ; DSP Interrupt enable bits
G_PITENA        equ   $00000040         ; PIT Interrupt enable bits
G_OPENA         equ   $00000080         ; Object Processor Interrupt enable bits
G_BLITENA       equ   $00000100         ; Blitter Interrupt enable bits
G_CPUCLR        equ   $00000200         ; CPU Interrupt clear bits
G_DSPCLR        equ   $00000400         ; DSP Interrupt clear bits
G_PITCLR        equ   $00000800         ; PIT Interrupt clear bits
G_OPCLR         equ   $00001000         ; Object Processor Interrupt clear bits
G_BLITCLR       equ   $00002000         ; Blitter Interrupt clear bits

;;;
;;; GPU Control/Status Register
;;;

GPUGO           equ   $00000001         ; Start and Stop the GPU
;GPUINT0        equ   $00000004         ; generate a GPU type 0 interrupt

G_CPULAT        equ   $00000040         ; Interrupt Latches
G_DSPLAT        equ   $00000080
G_PITLAT        equ   $00000100
G_OPLAT         equ   $00000200
G_BLITLAT       equ   $00000400

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       BLITTER REGISTERS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

A1_BASE         equ   $F02200           ; A1 Base Address
A1_FLAGS        equ   $F02204           ; A1 Control Flags
A1_CLIP         equ   $F02208           ; A1 Clipping Size
A1_PIXEL        equ   $F0220C           ; A1 Pixel Pointer
A1_STEP         equ   $F02210           ; A1 Step (Integer Part)
A1_FSTEP        equ   $F02214           ; A1 Step (Fractional Part)
A1_FPIXEL       equ   $F02218           ; A1 Pixel Pointer (Fractional)
A1_INC          equ   $F0221C           ; A1 Increment (Integer Part)
A1_FINC         equ   $F02220           ; A1 Increment (Fractional Part)
A2_BASE         equ   $F02224           ; A2 Base Address
A2_FLAGS        equ   $F02228           ; A2 Control Flags
A2_MASK         equ   $F0222C           ; A2 Address Mask
A2_PIXEL        equ   $F02230           ; A2 PIXEL
A2_STEP         equ   $F02234           ; A2 Step (Integer)

B_CMD           equ   $F02238           ; Command
B_COUNT         equ   $F0223C           ; Counters
B_SRCD          equ   $F02240           ; Source Data
B_DSTD          equ   $F02248           ; Destination Data
B_DSTZ          equ   $F02250           ; Destination Z
B_SRCZ1         equ   $F02258           ; Source Z (Integer)
B_SRCZ2         equ   $F02260           ; Source Z (Fractional)
B_PATD          equ   $F02268           ; Pattern Data
B_IINC          equ   $F02270           ; Intensity Increment
B_ZINC          equ   $F02274           ; Z Increment
B_STOP          equ   $F02278           ; Collision stop control

B_I3            equ   $F0227C           ; Blitter Intensity 3
B_I2            equ   $F02280           ; Blitter Intensity 2
B_I1            equ   $F02284           ; Blitter Intensity 1
B_I0            equ   $F02288           ; Blitter Intensity 0

B_Z3            equ   $F0228C           ; Blitter Z 3
B_Z2            equ   $F02290           ; Blitter Z 2
B_Z1            equ   $F02294           ; Blitter Z 1
B_Z0            equ   $F02298           ; Blitter Z 0

;;;
;;; BLITTER Command Register equates
;;;

SRCEN           equ   $00000001         ; d00:      source data read (inner loop)
SRCENZ          equ   $00000002         ; d01:      source Z read (inner loop)
SRCENX          equ   $00000004         ; d02:      source data read (realign)
DSTEN           equ   $00000008         ; d03:      dest data read (inner loop)
DSTENZ          equ   $00000010         ; d04:      dest Z read (inner loop)
DSTWRZ          equ   $00000020         ; d05:      dest Z write (inner loop)
CLIP_A1         equ   $00000040         ; d06:      A1 clipping enable
UPDA1F          equ   $00000100         ; d08:      A1 update step fraction
UPDA1           equ   $00000200         ; d09:      A1 update step
UPDA2           equ   $00000400         ; d10:      A2 update step
DSTA2           equ   $00000800         ; d11:      reverse usage of A1 and A2
GOURD           equ   $00001000         ; d12:      enable Gouraud shading
ZBUFF           equ   $00002000         ; d13:      polygon Z data updates
TOPBEN          equ   $00004000         ; d14:      intensity carry into byte
TOPNEN          equ   $00008000         ; d15:      intensity carry into nibble
PATDSEL         equ   $00010000         ; d16:      Select pattern data
ADDDSEL         equ   $00020000         ; d17:      diagnostic
                                        ; d18-d20:  Z comparator inhibit
ZMODELT         equ   $00040000         ;           source < destination
ZMODEEQ         equ   $00080000         ;           source = destination
ZMODEGT         equ   $00100000         ;           source > destination
                                        ; d21-d24:  Logic function control
LFU_NAN         equ   $00200000         ;           !source & !destination
LFU_NA          equ   $00400000         ;           !source & destination
LFU_AN          equ   $00800000         ;           source & !destination
LFU_A           equ   $01000000         ;           source & destination

CMPDST          equ   $02000000         ; d25: pixel compare pattern & dest
BCOMPEN         equ   $04000000         ; d26: bit compare write inhibit
DCOMPEN         equ   $08000000         ; d27: data compare write inhibit
BKGWREN         equ   $10000000         ; d28: data write back
BUSHI           equ   $20000000         ; d29: blitter priority
SRCSHADE        equ   $40000000         ; d30: shade src data w/IINC value

;;;
;;; The following are ALL 16 possible logical operations of the LFUs
;;;

LFU_ZERO        equ   $00000000         ; All Zeros
LFU_NSAND       equ   $00200000         ; NOT Source AND NOT Destination
LFU_NSAD        equ   $00400000         ; NOT Source AND Destination
LFU_NOTS        equ   $00600000         ; NOT Source
LFU_SAND        equ   $00800000         ; Source AND NOT Destination
LFU_NOTD        equ   $00A00000         ; NOT Destination
LFU_N_SXORD     equ   $00C00000         ; NOT (Source XOR Destination)
LFU_NSORND      equ   $00E00000         ; NOT Source OR NOT Destination
LFU_SAD         equ   $01000000         ; Source AND Destination
LFU_SXORD       equ   $01200000         ; Source XOR Destination
LFU_D           equ   $01400000         ; Destination
LFU_NSORD       equ   $01600000         ; NOT Source OR Destination
LFU_S           equ   $01800000         ; Source
LFU_SORND       equ   $01A00000         ; Source OR NOT Destination
LFU_SORD        equ   $01C00000         ; Source OR Destination
LFU_ONE         equ   $01E00000         ; All Ones

; These are some common combinations with less boolean names

LFU_REPLACE     equ   $01800000         ; Source REPLACEs destination
LFU_XOR         equ   $01200000         ; Source XOR with destination
LFU_CLEAR       equ   $00000000         ; CLEAR destination

;;;
;;; BLITTER Flags (A1 or A2) register equates
;;;

; Pitch d00-d01:
;       distance between pixel phrases
PITCH1          equ   $00000000         ; 0 phrase gap
PITCH2          equ   $00000001         ; 1 phrase gap
PITCH4          equ   $00000002         ; 3 phrase gap
PITCH3          equ   $00000003         ; 2 phrase gap

; Pixel d03-d05
;       bit depth (2^n)
PIXEL1          equ   $00000000         ; n = 0 
PIXEL2          equ   $00000008         ; n = 1 
PIXEL4          equ   $00000010         ; n = 2 
PIXEL8          equ   $00000018         ; n = 3 
PIXEL16         equ   $00000020         ; n = 4 
PIXEL32         equ   $00000028         ; n = 5 

; Z offset d06-d08
;       offset from phrase of pixel data from its corresponding
;       Z data phrases
ZOFFS0          equ   $00000000         ; offset = 0    UNUSED
ZOFFS1          equ   $00000040         ; offset = 1
ZOFFS2          equ   $00000080         ; offset = 2
ZOFFS3          equ   $000000C0         ; offset = 3
ZOFFS4          equ   $00000100         ; offset = 4
ZOFFS5          equ   $00000140         ; offset = 5
ZOFFS6          equ   $00000180         ; offset = 6
ZOFFS7          equ   $000001C0         ; offset = 7    UNUSED

; Width d09-d14
;       width used for address generation
;       This is a 6-bit floating point value in pixels
;       4-bit unsigned exponent
;       2-bit mantissa with implied 3rd bit of 1
WID2            equ   $00000800         ; 1.00 X 2^1  ( 4<<9)
WID4            equ   $00001000         ; 1.00 X 2^2  ( 8<<9)
WID6            equ   $00001400         ; 1.10 X 2^2  (10<<9)
WID8            equ   $00001800         ; 1.00 x 2^3  (12<<9)
WID10           equ   $00001A00         ; 1.01 X 2^3  (13<<9)
WID12           equ   $00001C00         ; 1.10 X 2^3  (14<<9)
WID14           equ   $00001E00         ; 1.11 X 2^3  (15<<9)
WID16           equ   $00002000         ; 1.00 X 2^4  (16<<9)
WID20           equ   $00002200         ; 1.01 X 2^4  (17<<9)
WID24           equ   $00002400         ; 1.10 X 2^4  (18<<9)
WID28           equ   $00002600         ; 1.11 X 2^4  (19<<9)
WID32           equ   $00002800         ; 1.00 X 2^5  (20<<9)
WID40           equ   $00002A00         ; 1.01 X 2^5  (21<<9)
WID48           equ   $00002C00         ; 1.10 X 2^5  (22<<9)
WID56           equ   $00002E00         ; 1.11 X 2^5  (23<<9)
WID64           equ   $00003000         ; 1.00 X 2^6  (24<<9)
WID80           equ   $00003200         ; 1.01 X 2^6  (25<<9)
WID96           equ   $00003400         ; 1.10 X 2^6  (26<<9)
WID112          equ   $00003600         ; 1.11 X 2^6  (27<<9)
WID128          equ   $00003800         ; 1.00 X 2^7  (28<<9)
WID160          equ   $00003A00         ; 1.01 X 2^7  (29<<9)
WID192          equ   $00003C00         ; 1.10 X 2^7  (30<<9)
WID224          equ   $00003E00         ; 1.11 X 2^7  (31<<9)
WID256          equ   $00004000         ; 1.00 X 2^8  (32<<9)
WID320          equ   $00004200         ; 1.01 X 2^8  (33<<9)
WID384          equ   $00004400         ; 1.10 X 2^8  (34<<9)
WID448          equ   $00004600         ; 1.11 X 2^8  (35<<9)
WID512          equ   $00004800         ; 1.00 X 2^9  (36<<9)
WID640          equ   $00004A00         ; 1.01 X 2^9  (37<<9)
WID768          equ   $00004C00         ; 1.10 X 2^9  (38<<9)
WID896          equ   $00004E00         ; 1.11 X 2^9  (39<<9)
WID1024         equ   $00005000         ; 1.00 X 2^10 (40<<9)
WID1280         equ   $00005200         ; 1.01 X 2^10 (41<<9)
WID1536         equ   $00005400         ; 1.10 X 2^10 (42<<9)
WID1792         equ   $00005600         ; 1.11 X 2^10 (43<<9)
WID2048         equ   $00005800         ; 1.00 X 2^11 (44<<9)
WID2560         equ   $00005A00         ; 1.01 X 2^11 (45<<9)
WID3072         equ   $00005C00         ; 1.10 X 2^11 (46<<9)
WID3584         equ   $00005E00         ; 1.11 X 2^11 (47<<9)

; X add control d16-d17
;       controls the update of the X pointer on each pass
;       round the inner loop
XADDPHR         equ   $00000000         ; 00 - add phrase width and truncate
XADDPIX         equ   $00010000         ; 01 - add pixel size (add 1)   
XADD0           equ   $00020000         ; 10 - add zero
XADDINC         equ   $00030000         ; 11 - add the increment

; Y add control d18
;       controls the update of the Y pointer within the inner loop.
;       it is overridden by the X add control if they are in add increment
YADD0           equ   $00000000         ; 00 - add zero
YADD1           equ   $00040000         ; 01 - add 1

; X sign d19
;       add or subtract pixel size if X add control = 01 (XADDPIX)
XSIGNADD        equ   $00000000         ; 0 - add pixel size
XSIGNSUB        equ   $00080000         ; 1 - subtract pixel size

; Y sign d20
;       add or subtract pixel size if Y add control = 01 (YADD1)
YSIGNADD        equ   $00000000         ; 0 - add 1
YSIGNSUB        equ   $00100000         ; 1 - sub 1


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       JERRY REGISTERS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

JPIT1           equ   $F10000           ; Timer 1 Pre-Scaler
JPIT2           equ   $F10002           ; Timer 1 Divider
JPIT3           equ   $F10004           ; Timer 2 Pre-Scaler
JPIT4           equ   $F10006           ; Timer 2 Divider

J_INT           equ   $F10020           ; Jerry Interrupt control (to TOM)
        
JOYSTICK        equ   $F14000           ; Joystick register and mute
JOYBUTS         equ   $F14002           ; Joystick register
CONFIG          equ   $F14002           ; Also has NTSC/PAL

SCLK            equ   $F1A150           ; SSI Clock Frequency
SMODE           equ   $F1A154           ; SSI Control

L_I2S           equ   $F1A148           ; I2S Serial Interface
R_I2S           equ   $F1A14C           ; I2S Serial Interface
LTXD            equ   $F1A148           ; Synonyms
RTXD            equ   $F1A14C
LRXD            equ   $F1A148           ; Synonyms
RRXD            equ   $F1A14C

R_DAC           equ   $F1A148           ; Swapped on Purpose!
L_DAC           equ   $F1A14C

ASICLK          equ   $F10034           ; Asynchronous Clock Register
ASICTRL         equ   $F10032           ; Asynchronous Control Register
ASISTAT         equ   $F10032           ; Asynchronous Status Register
ASIDATA         equ   $F10030           ; Asynchronous Data Register


;================================================================
; Synchronous Serial Interface
;================================================================

SSI_INTERNAL    equ   (1<<0)            ; When set the bit enables the serial clock and word strobe outputs.
SSI_WSEN        equ   (1<<2)            ; This bit enables the generation of word strobe pulses.
                                        ; When set Jerry produces a word strobe output which is alternately high for 16 clock cycles and low for 16 clock cycles.
                                        ; When cleared Jerry will not generate further high pulses. This bit is ignored when INTERNAL is cleared.
SSI_RISING      equ   (1<<3)            ; Enables interrupts on the rising edge of word strobe.
SSI_FALLING     equ   (1<<4)            ; Enables interrupts on the falling edge of word strobe.
SSI_EVERYWORD   equ   (1<<5)            ; Enables interrupts on the MSB of every word transmitted or received.

;================================================================
; UART Definitions (new in this file as of 24-Apr-95)
;================================================================

; UART control register Masks 
; All unused bits in the control register need to be written as zeros !	      
; With exception of U_CLRERR these are valid for read in ASISTAT, too	    

U_MODD          equ   (1<<0)            ; selects odd parity
U_MPAREN        equ   (1<<1)            ; enable parity
U_MTXOPOL       equ   (1<<2)            ; transmit output polarity (if set: active low)
U_MRXIPOL       equ   (1<<3)            ; receive input polarity (if set: invert input)
U_MTINTEN       equ   (1<<4)            ; enable transmitter interrupts
U_MRINTEN       equ   (1<<5)            ; enable reciever interrupts
U_MCLRERR       equ   (1<<6)            ; clear error (only use if U_SERIN is 
                                        ; inactive otherwise the UART locks up.
                                        ; By default input is active low. This
                                        ; depends on U_MRXIPOL) 
U_MTXBRK        equ   (1<<14)           ; transmit break

; UART control register (ONLY) bit numbers
U_CLRERR        equ   6                 ;

; UART control AND status register (SHARED) bit numbers 

U_ODD           equ   0                 ; selects odd parity
U_PAREN         equ   1                 ; enable parity
U_TXOPOL        equ   2                 ; transmit output polarity (if set: active low)
U_RXIPOL        equ   3                 ; receive input polarity (if set: invert input)
U_TINTEN        equ   4                 ; enable transmitter interrupts
U_RINTEN        equ   5                 ; enable reciever interrupts
U_TXBRK         equ   14                ; transmit break

; UART status register (ONLY) bit numbers

U_ERR           equ   15                ; error condition exists
U_SERIN         equ   13                ; state of UART1 Pin (serial input data)
U_OE            equ   11                ; overrun error
U_FE            equ   10                ; frame error
U_PE            equ   9                 ; parity error
U_TBE           equ   8                 ; transitter buffer empty
U_RBF           equ   7                 ; receiver buffer full


;;;
;;; Jerry Interrupt Control Flags
;;;

J_EXTENA        equ   $0001             ; Enable external interrupts
J_DSPENA        equ   $0002             ; Enable DSP interrupts
J_TIM1ENA       equ   $0004             ; Enable Timer 1 interrupts
J_TIM2ENA       equ   $0008             ; Enable Timer 2 interrupts
J_ASYNENA       equ   $0010             ; Enable Asyncronous Serial interrupts
J_SYNENA        equ   $0020             ; Enable Syncronous Serial interrupts

J_EXTCLR        equ   $0100             ; Clear pending external interrupts
J_DSPCLR        equ   $0200             ; Clear pending DSP interrupts
J_TIM1CLR       equ   $0400             ; Clear pending Timer 1 interrupts
J_TIM2CLR       equ   $0800             ; Clear pending Timer 2 interrupts
J_ASYNCLR       equ   $1000             ; Clear pending Asynch. Serial interrupts
J_SYNCLR        equ   $2000             ; Clear pending Synch. Serial interrupts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;     JERRY Joystick equates
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Bits when LONGword is formatted as below (from JOYTEST\JT_LOOP.S).
;
; Format: xxApxxBx RLDU147* xxCxxxox 2580369#

JOY_UP          equ   20                ;joypad
JOY_DOWN        equ   21
JOY_LEFT        equ   22
JOY_RIGHT       equ   23

FIRE_A          equ   29                ;fire buttons
FIRE_B          equ   25
FIRE_C          equ   13
OPTION          equ   9
PAUSE           equ   28

KEY_STAR        equ   16                ;keypad
KEY_7           equ   17
KEY_4           equ   18
KEY_1           equ   19

KEY_0           equ   4
KEY_8           equ   5
KEY_5           equ   6
KEY_2           equ   7

KEY_HASH        equ   0
KEY_9           equ   1
KEY_6           equ   2
KEY_3           equ   3

ANY_JOY         equ   $00F00000         ; AND joyedge with this...
                                        ; joypad was pressed if result is not 0
ANY_FIRE        equ   $32002200         ; AND joyedge with this...
                                        ; A,B C, Option or Pause was pressed
                                        ; if result is not 0
ANY_KEY         equ   $000F00FF         ; AND joyedge with this... 123456789*0#
                                        ; was pressed if result is not 0

;;;
;;;     ROM Tables built into Jerry - 128 samples each
;;;     16 bit samples sign extended to 32
;;;

ROM_TABLE       equ   $F1D000           ; Base of tables

ROM_TRI         equ   $F1D000           ; A triangle wave
ROM_SINE        equ   $F1D200           ; Full amplitude SINE
ROM_AMSINE      equ   $F1D400           ; Linear (?) ramp SINE
ROM_12W         equ   $F1D600           ; SINE(X)+SINE(2*X) : (was ROM_SINE12W)
ROM_CHIRP16     equ   $F1D800           ; SHORT SWEEP
ROM_NTRI        equ   $F1DA00           ; Triangle w/NOISE
ROM_DELTA       equ   $F1DC00           ; Positive spike
ROM_NOISE       equ   $F1DE00           ; Guess

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       JERRY Registers (DSP)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

D_FLAGS         equ   $F1A100           ; DSP Flags
D_MTXC          equ   $F1A104           ; DSP Matrix Control
D_MTXA          equ   $F1A108           ; DSP Matrix Address
D_END           equ   $F1A10C           ; DSP Data Organization
D_PC            equ   $F1A110           ; DSP Program Counter
D_CTRL          equ   $F1A114           ; DSP Operation Control/Status
D_MOD           equ   $F1A118           ; DSP Modulo Instruction Mask
D_REMAIN        equ   $F1A11C           ; DSP Division Remainder
D_DIVCTRL       equ   $F1A11C           ; DSP Divider control
D_MACHI         equ   $F1A120           ; DSP Hi byte of MAC operations
D_RAM           equ   $F1B000           ; DSP Internal RAM
D_ENDRAM        equ   D_RAM+(8*1024)    ; 8K bytes

;;;
;;; JERRY Flag Register equates
;;;

D_CPUENA        equ   $00000010         ; CPU Interrupt Enable Bit
D_I2SENA        equ   $00000020         ; I2S Interrupt Enable Bit
D_TIM1ENA       equ   $00000040         ; Timer 1 Interrupt Enable Bit
D_TIM2ENA       equ   $00000080         ; Timer 2 Interrupt Enable Bit
D_EXT0ENA       equ   $00000100         ; External Interrupt 0 Enable Bit
D_EXT1ENA       equ   $00010000         ; External Interrupt 1 Enable Bit

D_CPUCLR        equ   $00000200         ; CPU Interrupt Clear Bit
D_I2SCLR        equ   $00000400         ; I2S Interrupt Clear Bit
D_TIM1CLR       equ   $00000800         ; Timer 1 Interrupt Clear Bit
D_TIM2CLR       equ   $00001000         ; Timer 2 Interrupt Clear Bit
D_EXT0CLR       equ   $00002000         ; External Interrupt 0 Clear Bit
D_EXT1CLR       equ   $00020000         ; External Interrupt 1 Clear Bit

;;;
;;; JERRY Control/Status Register
;;;

DSPGO           equ   $00000001         ; Start DSP
DSPINT0         equ   $00000004         ; Generate a DSP Interrupt 0

D_CPULAT        equ   $00000040         ; Interrupt Latches
D_I2SLAT        equ   $00000080
D_TIM1LAT       equ   $00000100
D_TIM2LAT       equ   $00000200
D_EXT1LAT       equ   $00000400
D_EXT2LAT       equ   $00010000

;;;
;;; JERRY Modulo Instruction Masks
;;;

MODMASK2        equ   $FFFFFFFE         ; 2 byte circular buffer
MODMASK4        equ   $FFFFFFFC         ; 4 byte circular buffer
MODMASK8        equ   $FFFFFFF8         ; 8 byte circular buffer
MODMASK16       equ   $FFFFFFF0         ; 16 byte circular buffer
MODMASK32       equ   $FFFFFFE0         ; 32 byte circular buffer
MODMASK64       equ   $FFFFFFC0         ; 64 byte circular buffer
MODMASK128      equ   $FFFFFF80         ; 128 byte circular buffer
MODMASK256      equ   $FFFFFF00         ; 256 byte circular buffer
MODMASK512      equ   $FFFFFE00         ; 512 byte circular buffer
MODMASK1K       equ   $FFFFFC00         ; 1k circular buffer
MODMASK2K       equ   $FFFFF800         ; 2k circular buffer
MODMASK4K       equ   $FFFFF000         ; 4k circular buffer
MODMASK8K       equ   $FFFFE000         ; 8k circular buffer
MODMASK16K      equ   $FFFFC000         ; 16k circular buffer
MODMASK32K      equ   $FFFF8000         ; 32k circular buffer
MODMASK64K      equ   $FFFF0000         ; 64k circular buffer
MODMASK128K     equ   $FFFE0000         ; 128k circular buffer
MODMASK256K     equ   $FFFC0000         ; 256k circular buffer
MODMASK512K     equ   $FFF80000         ; 512k circular buffer
MODMASK1M       equ   $FFF00000         ; 1M circular buffer
        
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; SHARED equates for TOM (GPU) and JERRY (DSP)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;
;;; Control/Status Registers
;;;

RISCGO          equ   $00000001         ; Start GPU or DSP
CPUINT          equ   $00000002         ; Allow the GPU/DSP to interrupt CPU
FORCEINT0       equ   $00000004         ; Cause an INT 0 on GPU or DSP
SINGLE_STEP     equ   $00000008         ; Enter SINGLE_STEP mode
SINGLE_GO       equ   $00000010         ; Execute one instruction

REGPAGE         equ   $00004000         ; Register Bank Select
DMAEN           equ   $00008000         ; Enable DMA LOAD and STORE

;;;
;;; Flags Register
;;;

ZERO_FLAG       equ   $00000001         ; ALU Zero Flag
CARRY_FLAG      equ   $00000002         ; ALU Carry Flag
NEGA_FLAG       equ   $00000004         ; ALU Negative Flag

IMASK           equ   $00000008         ; Interrupt Service Mask

;;;
;;; Matrix Control Register
;;;

MATRIX3         equ   $00000003         ; use for 3x1 Matrix
MATRIX4         equ   $00000004         ; etc...
MATRIX5         equ   $00000005
MATRIX6         equ   $00000006
MATRIX7         equ   $00000007
MATRIX8         equ   $00000008
MATRIX9         equ   $00000009
MATRIX10        equ   $0000000A
MATRIX11        equ   $0000000B
MATRIX12        equ   $0000000C
MATRIX13        equ   $0000000D
MATRIX14        equ   $0000000E
MATRIX15        equ   $0000000F
        
MATROW          equ   $00000000         ; Access Matrix by Row
MATCOL          equ   $00000010         ; Access Matrix by Column

;;;
;;; Data Organisation Register
;;;

BIG_IO          equ   $00010001         ; Use 32-bit registers as big-endian
BIG_PIX         equ   $00020002         ; Pixel organisation is big-endian
BIG_INST        equ   $00040004         ; Word program fetches are big-endian

;;;
;;; Divide Unit Control
;;;

DIV_OFFSET      equ   $00000001         ; Divide 16.16 values if set

