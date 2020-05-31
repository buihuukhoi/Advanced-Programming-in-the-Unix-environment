;mov eax, [0x600000]
;sub eax, 0x20
;mov [0x600001], eax
;done:

mov eax, [0x600000]
and eax, 0xdf
mov [0x600001], eax
done:
