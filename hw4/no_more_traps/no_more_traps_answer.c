#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>


void errquit(const char *msg) {
	perror(msg);
	exit(-1);
}

int main(int argc, char *argv[]) {
	pid_t child;
	int status;
	
	if((child = fork()) < 0) errquit("fork");
	if(child == 0) { // child
		if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) errquit("ptrace_child");
		execlp("./no_more_traps", "./no_more_traps", NULL);
		errquit("execlp");
	} else { //parent
		int status;
		int fd;

		if(waitpid(child, &status, 0) < 0) errquit("waitpid");
		assert(WIFSTOPPED(status));
		ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
		/* continue the execution */
		ptrace(PTRACE_CONT, child, 0, 0);
		
		// read opcodes from txt file
		if((fd = open("no_more_traps.txt", O_RDONLY)) == -1)
			errquit("open");

		while(waitpid(child, &status, 0) > 0) {
			unsigned long opcode, patched_opcode;
			struct user_regs_struct regs;

			// only wait for SIGTRAP (from int3)
			if(WSTOPSIG(status) != SIGTRAP) continue;

			// get all registers from callee
			if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("ptrace(GETREGS)");
			
			// prepare opcode for patching
			char *s = (char*) alloca(sizeof(char)*2);
			read(fd, s, 2);
			sscanf(s, "%lx", &patched_opcode);

			// patch opcode to int3 / 0xcc
			opcode = ptrace(PTRACE_PEEKTEXT, child, regs.rip-1, 0);
			if (ptrace(PTRACE_POKETEXT, child, regs.rip-1,
						(opcode & 0xffffffffffffff00 | patched_opcode)))
				errquit("ptrace(POKETEXT)");
			
			// change rip to point to the patched opcode
			regs.rip = regs.rip - 1;
			if (ptrace(PTRACE_SETREGS, child, 0, &regs)) errquit("ptrace(SETREGS)");	
			ptrace(PTRACE_CONT, child, 0, 0);
		}

		close(fd);
	}
	return 0;
}
