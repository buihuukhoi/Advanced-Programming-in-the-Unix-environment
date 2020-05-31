	mov bx, ax

	mov ecx, 0
loop1:	cmp ecx, 15
	jg end

	shl bx, 1
	jc label1
	mov DWORD PTR [0x600000 + ecx], 0x30
	jmp label2
label1: mov DWORD PTR [0x600000 + ecx], 0x31

label2:	inc ecx
	jmp loop1

end:
done:
