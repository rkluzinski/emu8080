        .org	0x100
        lxi     sp, 0x1000
        lxi     d, msg
        mvi     c, 9
        call    5
        mvi     a, 0
        call    0
msg:    db      'Hello, World!',10,'$'