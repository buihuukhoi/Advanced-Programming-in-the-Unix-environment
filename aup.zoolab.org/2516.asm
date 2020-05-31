mov eax, [0x600000]
mov ebx, [0x600000]
mov ecx, [0x600000]
SHL eax, 5
SHL ebx, 2
SHL ecx, 1
sub eax, ebx
sub eax, ecx
mov [0x600004], eax
done:

