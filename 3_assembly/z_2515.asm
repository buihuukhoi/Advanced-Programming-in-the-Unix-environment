;===== THE CODE
;    call   a
;    jmp    exit
;
;a:  ; function a - read ret-addr in rax
;    pop    rax
;    push   rax
;    ret
;exit:
;======
;======
;Enter your codes: (type 'done:' when done)
	call 	a
	jmp 	exit

a: ; function a - read ret-addr in rax
	pop 	rax
	push 	rax
	ret
exit:
done:
