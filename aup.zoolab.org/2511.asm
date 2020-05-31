	mov	eax, 0
	sub	eax, [0x600000]
	mov	ebx, [0x600004]
	imul	ebx
	add	eax, [0x600008]
done:

