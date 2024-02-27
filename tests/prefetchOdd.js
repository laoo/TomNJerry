    include "jaghard.inc"
    include "help.mac"

    dsp
rNop    reg 99
rFQ     reg 99
rTmpl   reg 99
rLoop   reg 99

    run     D_RAM

    movei   #template,r0          ;the address of template to write
    movei   #nop_field,r14        ;the address of field of nops that will be written to
    load    (r0),rTmpl            ;loading a template to write
    load    (r14),rNop            ;loading two nops
    movei   #flush_queue,rFQ      ;address where the test starts, it will be jumped to to flush the queue
    movei   #loop,rLoop           ;loop address
    movei   #-4,r0                ;we'll start from zero on the first run
    moveq   #0,r1                 ;flag to be changed

loop:
    cmpq    #1,r1                 ;check whether modfied code kicked-in
    jr      ne,skip               ;skip if not
    nop
    subq    #4,r0                 ;if yes - the r0 have the offset that is outside of the queue, hence subtracting to get the actual size
    STOP_DSP

skip
    addq    #4,r0                 ;preparing offset for next loop count
    jump    (rFQ)                 ;flishing the queue
    nop

template:
    moveq     #1,r1               ;the template code that will be written to field of nops to change the flag
    nop
    nop

flush_queue:                      ;it's on odd address
    store     rTmpl,(r14+r0)      ;the actual automodification under given offset


nop_field:                        ;a field of nops that starts just after the store
    rept 10
    nop
    endr

    store     rNop,(r14+r0)       ;reverting to original code
    jump      (rLoop)             ;and looping
    nop


