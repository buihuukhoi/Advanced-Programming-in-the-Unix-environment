	call 	a
	jmp 	exit

a: ; function a - read ret-addr in rax
	pop 	rax
	push 	rax
	ret
exit:
done:
