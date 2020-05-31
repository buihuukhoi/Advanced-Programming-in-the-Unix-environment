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

