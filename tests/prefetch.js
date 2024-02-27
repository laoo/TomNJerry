    include "jaghard.inc"
    include "help.mac"

    dsp
rNop    reg 99
rFQ     reg 99
rTmpl   reg 99
rLoop   reg 99
DONE    reg 99
ptr_s   reg 99
ptr     reg 14

    run     D_RAM

    movei   #done,DONE
    movei   #template,r0          ;the address of template to write
    movei   #nop_field,r14        ;the address of field of nops that will be written to
    move    r14,ptr_s
    load    (r0),rTmpl            ;loading a template to write
    load    (r14),rNop            ;loading two nops
    movei   #flush_queue,rFQ      ;address where the test starts, it will be jumped to to flush the queue
    movei   #loop,rLoop           ;loop address
    subq    #4,ptr
    moveq   #0,r1                 ;flag to be changed

loop:
    cmpq    #1,r1                 ;check whether modfied code kicked-in
    jr      ne,skip               ;skip if not
    nop
    subq    #4,ptr                 ;if yes - the r0 have the offset that is outside of the queue,
    jump    (DONE)          ; hence subtracting to get the actual size
    nop
skip
    jump    (rFQ)                 ;flishing the queue
    addq    #4,ptr                 ;preparing offset for next loop count
    nop
    nop

template:
    moveq   #1,r1               ;the template code that will be written to field of nops to change the flag
    nop
    nop


flush_queue:                      ;it's on odd address
    store   rTmpl,(ptr)      ;the actual automodification under given offset

nop_field:                        ;a field of nops that starts just after the store
    rept 10
    nop
    endr

    store   rNop,(r14)       ;reverting to original code
    jump    (rLoop)             ;and looping
    nop

done:
    sub     ptr_s,ptr
    move    ptr,r0
 IF nop_field & 3
    echo "%Hnop_field not aligned"
 ENDIF
    echo "%Htemplate"
    STOP_DSP

