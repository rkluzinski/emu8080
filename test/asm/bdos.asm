        out 0
        nop
        nop
        nop
        mvi     a,2
        cmp     c
        jz      chrOut
        mvi     a,9
        cmp     c
        jz      strOut
        jmp     0
chrOut: mov     a, e
        out     1
        ret
strOut: ldax    de
        cpi     0x24
        rz 
        out     1
        inx     de
        jmp     strOut