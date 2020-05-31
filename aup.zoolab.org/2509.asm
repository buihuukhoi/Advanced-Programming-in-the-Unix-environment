;loop15:
;	str1 is a string contains 15 lowercase and uppercase alphbets.
;	implement a loop to convert all alplabets to lowercase,
;	and store the result in str2.
;======
;      str1 @ 0x600000-600010
;      str2 @ 0x600010-600020
;======
;Enter your codes: (type 'done:' when done)
	mov eax, 0
loop1:	cmp eax, 15
	jg label1

	mov ebx, 0x600000
	add ebx, eax
	mov cl, [ebx]

	cmp cl, 0x41
	jl label2
	cmp cl, 0x5A
	jg label2
	add cl, 0x20

label2: mov edx, 0x600010
	add edx, eax
	mov [edx], cl

	inc eax
	jmp loop1

label1:
done:
