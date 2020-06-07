import sys
from pwn import *
import binascii as b
r = remote('aup.zoolab.org', 2530)
for i in range(10):
	mess = r.recvuntil('Your answer: ').decode('utf-8').split()
	question = mess[-3]
	question = b.a2b_hex(question)
	context.arch = "x86_64"
	dis_asm = disasm(question).split('\n')
	tmp_anwser = []
	for element in dis_asm:
		instruction = element.split('          ')[-1].split()
		tmp_anwser.append(' '.join(instruction))
	anwser = '\n'.join(tmp_anwser)
	anwser = anwser.encode('utf-8')
	r.sendline(b.b2a_hex(anwser).decode('utf-8'))
r.interactive()
