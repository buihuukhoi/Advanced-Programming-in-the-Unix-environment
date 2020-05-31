;math2: signed arithmetic
;	eax = (-var1 * var2) + var3
;======
;      var1 @ 0x600000-600004
;      var2 @ 0x600004-600008
;      var3 @ 0x600008-60000c
;======
;Enter your codes: (type 'done:' when done)
	mov	eax, 0
	sub	eax, [0x600000]
	mov	ebx, [0x600004]
	imul	ebx
	add	eax, [0x600008]
done:

