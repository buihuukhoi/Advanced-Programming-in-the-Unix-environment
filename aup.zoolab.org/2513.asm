mov eax, 0
sub eax, [0x600004]
cdq
mov ebx, [0x600008]
idiv ebx
mov ebx, edx

mov eax, [0x600000]
mov ecx, 0 
sub ecx, 5
imul ecx

cdq
idiv ebx
mov [0x60000c], eax
done:
