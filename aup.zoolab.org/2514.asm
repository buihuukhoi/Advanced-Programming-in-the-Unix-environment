mov ecx, [0x600008]
sub ecx, ebx

mov eax, [0x600000]
mov ebx, 0
sub ebx, [0x600004]
imul ebx

cdq
idiv ecx

mov [0x600008], eax
done:

