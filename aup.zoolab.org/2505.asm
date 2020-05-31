;dispbin:
;	given a number in AX, store the corresponding bit string in str1.
;	for example, if AX = 0x1234, the result should be:
;	str1 = 0001001000111000
;======
;      str1 @ 0x600000-600014
;======
;Enter your codes: (type 'done:' when done)
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
