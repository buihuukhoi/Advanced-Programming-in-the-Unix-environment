;bubble: bubble sort for 10 integers
;======
;      a[0] @ 0x600000-600004
;      a[1] @ 0x600004-600008
;      a[2] @ 0x600008-60000c
;      a[3] @ 0x60000c-600010
;      a[4] @ 0x600010-600014
;      a[5] @ 0x600014-600018
;      a[6] @ 0x600018-60001c
;      a[7] @ 0x60001c-600020
;      a[8] @ 0x600020-600024
;      a[9] @ 0x600024-600028
;======
;Enter your codes: (type 'done:' when done)
       xor ECX, ECX
loop1:
        xor EDX, EDX
loop2:
        mov EAX, [EDX*4+0x600000]
        mov EBX, [EDX*4+0x600000+4]

        cmp EAX, EBX
        jle label1

        mov [EDX*4+0x600000], EBX
        mov [EDX*4+0x600000+4], EAX

label1:
        inc EDX
        mov EAX, 9
        sub EAX, ECX
        cmp EDX, EAX
        jl loop2

        inc ECX
        cmp ECX, 9
        jl loop1
done:

