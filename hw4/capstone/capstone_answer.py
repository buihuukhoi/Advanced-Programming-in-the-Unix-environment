import sys
from pwn import *
import binascii as b

r = remote('aup.zoolab.org', 2530)
while(True):
	mess = r.recvline().decode('utf-8')
	if '>>>' in mess:
		question = mess.split()[-1]
		question = b.a2b_hex(question)
		context.arch = "x86_64"
		dis_asm = disasm(question).split('\n')
		tmp_anwser = []
		for element in dis_asm:
			instruction = element.split('      ')[-1].split()
			tmp_anwser.append(' '.join(instruction))
		anwser = '\n'.join(tmp_anwser)
		anwser = anwser.encode('utf-8')
		r.sendline(b.b2a_hex(anwser).decode('utf-8'))
		continue
	if 'ASM' in mess:
		print(mess)
		break

r.interactive()
