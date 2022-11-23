#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

static unsigned char data[] = {
...
};
static char output[64];

static int traced = 1;

int f(int n) { 
//	commented out
//	commented out
//	commented out
//	commented out
//	commented out
//	commented out
//	commented out
//	commented out
//	commented out
//	commented out
}

void handler(int s) {
	traced = 0;
	signal(SIGTRAP, SIG_DFL);
	printf("trace me, please.\n");
	exit(0);
}

int main(int argc, char *argv[]) {
	pid_t child;
	int i;
	char buf[64];
	signal(SIGTRAP, handler);
	raise(SIGTRAP);
	printf("traced\n");
	for(i = 0; i < 37; i++) {
		output[i] = data[1337 + f(i)];
	}
	output[i] = '\0';
	return 0;
}

