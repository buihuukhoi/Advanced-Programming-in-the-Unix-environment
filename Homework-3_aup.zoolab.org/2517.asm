;posneg: test if registers are positive or negative.
;	if ( eax >= 0 ) { var1 = 1 } else { var1 = -1 }
;	if ( ebx >= 0 ) { var2 = 1 } else { var2 = -1 }
;	if ( ecx >= 0 ) { var3 = 1 } else { var3 = -1 }
;	if ( edx >= 0 ) { var4 = 1 } else { var4 = -1 } 
;======
;      var1 @ 0x600000-600004
;      var2 @ 0x600004-600008
;      var3 @ 0x600008-60000c
;      var4 @ 0x60000c-600010
;======
;Enter your codes: (type 'done:' when done)
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
