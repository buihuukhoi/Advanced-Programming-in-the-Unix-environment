#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdarg.h>
#include <string.h>

void handleOptionExecuteCmd(int argc, char *argv[])
{
    char *soPath = "./sandbox.so";
    char *baseDir = ".";
    int opt;

    while ((opt = getopt(argc, argv, "p:d:-:")) != -1) {
        switch (opt) {
        case 'p':
//            printf("option p: ok with artument = %s\n", optarg);
            soPath = optarg;
            break;
        case 'd':
//            printf("option d: ok with argument = %s\n", optarg);
            baseDir = optarg;
            break;
        default: // 'other cases'
            fprintf(stderr, "Usage: %s [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n\
            -p: set the path to sandbox.so, default = ./sandbox.so\n\
            -d: the base directory that is allowed to access, default = .\n\
            --: separate the arguments for sandbox and for the executed command\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "no command given.\n");
        exit(EXIT_FAILURE);
    }

    int MaxArgs = 31;
    char *array[MaxArgs +1];

    argc -= optind;
	argv += optind;

    for(int i = 0; i < argc; i++) {
        array[i] = argv[i];
//		printf("argv[%d] = %s\n", optind + i, argv[i]);
	}
    array[argc] = (char *)(0);

    char soPathEnv[sizeof("LD_PRELOAD=") + strlen(soPath)];
    sprintf(soPathEnv, "LD_PRELOAD=%s", soPath);
    char baseDirEnv[sizeof("BASE_DIR=") + strlen(baseDir)];
    sprintf(baseDirEnv, "BASE_DIR=%s", baseDir);

    char *envp[] = {soPathEnv, baseDirEnv, NULL};
    if (execvpe(argv[0], array, envp) == -1)
        perror(argv[0]);
}

int main(int argc, char *argv[])
{
    handleOptionExecuteCmd(argc, argv);
    return 0;
}
