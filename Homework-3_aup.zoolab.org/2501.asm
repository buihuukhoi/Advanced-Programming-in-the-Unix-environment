;addsub2:
;	final = val1 + val2 - val3
;======
;      val1 @ 0x600000-600004
;      val2 @ 0x600004-600008
;      val3 @ 0x600008-60000c
;     final @ 0x60000c-600010
;======
;Enter your codes: (type 'done:' when done)
	mov	eax, [0x600000]
	add	eax, [0x600004]
	sub	eax, [0x600008]
	mov	[0x60000c], eax
done:

