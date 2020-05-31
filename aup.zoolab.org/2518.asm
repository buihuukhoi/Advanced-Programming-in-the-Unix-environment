	mov	rbx, 0				;use rbx to save 2*f(n-1) + 3*f(n-2)
	mov	rcx, 21				;rcx = n
	push 	rcx
	call 	f
	add 	rsp, 8
	jmp 	exit

f:	;funcion
	push 	rbp
	mov 	rbp, rsp
	
	mov	rcx, QWORD PTR [rbp + 16]	;rcx = n
	cmp 	rcx, 0
	jle 	f0

	cmp 	rcx, 1
	je 	f1

	push	rbx				;save rbx in stack
	
	sub	rcx, 1				;rcx = n-1
	push 	rcx
	call 	f				;f(n-1)
	add 	rsp, 0x8
	
	add	rax, rax			;rax = 2*f(n-1)
	mov 	rbx, rax			;rbx = 2*f(n-1)

	mov	rcx, QWORD PTR [rbp + 16]
	sub 	rcx, 2				;rcx = n-2
	push 	rcx
	call 	f				;f(n-2)
	add 	rsp, 0x8

	mov	rcx, 3
	mul	rcx				;rax = 3*f(n-2)
	add	rbx, rax			;rbx = 2*f(n-1) + 3*f(n-2)
	mov	rax, rbx			;rax = 2*f(n-1) + 3*f(n-2)

	pop	rbx				;restore rbx from stack
	jmp	return				;pop rip

f0:	mov 	rax, 0x0
	jmp 	return
f1:	mov 	rax, 0x1
	jmp 	return

return:	mov	rsp, rbp
	pop 	rbp
	ret

exit:
done:
