cond1:	cmp eax, 0
	jge label1
	mov DWORD PTR [0x600000], -1
	jmp cond2
label1:
	mov DWORD PTR [0x600000], 1

cond2:	cmp ebx, 0
	jge label2
	mov DWORD PTR [0x600004], -1
	jmp cond3
label2:
	mov DWORd PTR [0x600004], 1

cond3:	cmp ecx, 0
        jge label3
        mov DWORD PTR [0x600008], -1
	jmp cond4
label3:
	mov DWORD PTR [0x600008], 1

cond4:	cmp edx, 0
        jge label4
        mov DWORD PTR [0x60000c], -1
	jmp end
label4:
	mov DWORD PTR [0x60000c], 1

end:
done:
