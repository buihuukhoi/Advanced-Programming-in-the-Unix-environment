	cmp	ch, 0x41
;	jl	cond2
	jl	exit
	cmp	ch, 0x5a
	jg	cond2
	add	ch, 0x20
	jmp	exit

cond2:	cmp	ch, 0x61
	jl	exit
	cmp	ch, 0x7a
	jg	exit
	sub	ch, 0x20

exit:
done:
