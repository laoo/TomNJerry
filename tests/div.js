    include "jaghard.inc"

    dsp

    run     D_RAM
  

  movei    #D_FLAGS, r0; set flags
      moveta    r0, r3; preset restult
      moveta    r0, r0; flags also in bank 2
    load(r0), r1; get flags
      moveta    r1, r1; original to bank 2
      bset    #14, r1

      movei    #$12345678, r10
      movei    #$123, r11
      div    r11, r10; start divid
      store    r1, (r0); 2. bank
      ; nop; wait for effect
      movefa    r10, r3 ; get "result"
      store    r1, (r0); switch back to bank 1
  nop
      movefa    r3, r0; now get result
      nop

