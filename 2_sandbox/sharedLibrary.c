#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <dirent.h>


#define LibC "libc.so.6"
#define monitor_sandbox "monitor_sandbox.txt"
#define controllingTerminal "/dev/tty"
#define MAXARGS 31

static int (*old_chdir)(const char *path) = NULL;
static int (*old_chmod)(const char *pathname, mode_t mode) = NULL;
static int (*old_chown)(const char *pathname, uid_t owner, gid_t group) = NULL;
static int (*old_creat)(const char *pathname, mode_t mode) = NULL;
static FILE* (*old_fopen)(const char *pathname, const char *mode) = NULL;
static int (*old_link)(const char *path1, const char *path2) = NULL;
static int (*old_mkdir)(const char *path, mode_t mode) = NULL;
static int (*old_open)(const char *path, int oflag, ... ) = NULL;
static int (*old_openat)(int fd, const char *path, int oflag, ...) = NULL;
static DIR* (*old_opendir)(const char *name) = NULL;
static ssize_t (*old_readlink)(const char *restrict path, char *restrict buf, size_t bufsize) = NULL;
static int (*old_remove)(const char *pathname) = NULL;
static int (*old_rmdir)(const char *path) = NULL;
static int (*old___xstat)(int ver, const char *path, struct stat *stat_buf) = NULL;
static int (*old_symlink)(const char *path1, const char *path2) = NULL;
static int (*old_unlink)(const char *path) = NULL;

ino_t pathInode = -1;
FILE* ErrFile = NULL;
int CtrTermFile = -1;


/* MACRO */

#define HANDLE_PERMISSION_ERR(funcName, pathName)\
    if(!checkPathUnderBaseDir(pathName)){\
        handlePermissionErr(#funcName, pathName);\
        return returnedValue;\
    }

#define HANDLE_OLD_FUNC(name)\
    if(old_##name == NULL){\
        void *handle = dlopen(LibC, RTLD_LAZY);\
        if(handle != NULL){\
            if ((old_##name = dlsym(handle, #name)) == NULL){\
                if (ErrFile != NULL)\
                    fprintf(ErrFile, "[sandbox] " #name " failed\n");\
                fprintf(stderr, "[sandbox] " #name " failed\n");\
                exit(EXIT_FAILURE);\
            }\
        }\
        else{\
            if (ErrFile != NULL)\
                fprintf(ErrFile, "[sandbox] cannot open %s\n", LibC);\
            fprintf(stderr, "[sandbox] cannot open %s\n", LibC);\
            exit(EXIT_FAILURE);\
        }\
    }
    
#define EXECUTE_OLD_FUNCTION_1_PARAM(name, formatRetStr, param1, formatStr1)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1);\
    openMonitorFile();\
    if (ErrFile != NULL)\
        fprintf(ErrFile, "[sandbox] " #name "(" #formatStr1 ") = " #formatRetStr "\n", param1, returnedValue);\
    return returnedValue;
    
#define EXECUTE_OLD_FUNCTION_2_PARAM(name, formatRetStr, param1, formatStr1, param2, formatStr2)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1, param2);\
    openMonitorFile();\
    if (ErrFile != NULL)\
        fprintf(ErrFile, "[sandbox] " #name "(" #formatStr1 ", " #formatStr2 ") = " #formatRetStr "\n", param1, param2, returnedValue);\
    return returnedValue;

#define EXECUTE_OLD_FUNCTION_3_PARAM(name, formatRetStr, param1, formatStr1, param2, formatStr2, param3, formatStr3)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1, param2, param3);\
    openMonitorFile();\
    if (ErrFile != NULL)\
        fprintf(ErrFile, "[sandbox] " #name "(" #formatStr1 ", " #formatStr2 ", " #formatStr3 ") = " #formatRetStr "\n", param1, param2, param3, returnedValue);\
    return returnedValue;

#define EXECUTE_OLD_FUNCTION_4_PARAM(name, formatRetStr, param1, formatStr1, param2, formatStr2, param3, formatStr3, param4, formatStr4)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1, param2, param3, param4);\
    openMonitorFile();\
    if (ErrFile != NULL)\
        fprintf(ErrFile, "[sandbox] " #name "(" #formatStr1 ", " #formatStr2 ", " #formatStr3 ", " #formatStr4 ") = " #formatRetStr "\n", param1, param2, param3, param4, returnedValue);\
    return returnedValue;


__attribute__((constructor)) static void beforeMain(){
    HANDLE_OLD_FUNC(fopen);
    if (ErrFile == NULL){
        if((ErrFile = old_fopen(monitor_sandbox, "ab")) == NULL){
            char myErrMess[1024];
            snprintf(myErrMess, sizeof(myErrMess), "[sandbox] fopen(\"%s\")", monitor_sandbox);
            perror(myErrMess);
        }
    }
    if (CtrTermFile == -1){
        HANDLE_OLD_FUNC(open);
        if((CtrTermFile = old_open(controllingTerminal, O_WRONLY)) == -1){
            char myErrMess[1024];
            snprintf(myErrMess, sizeof(myErrMess), "[sandbox] fopen(\"%s\")", controllingTerminal);
            perror(myErrMess);
        }
        else{
            dup2(CtrTermFile, 2);
        }
    }
}

void openMonitorFile(){
    if (ErrFile == NULL){
        HANDLE_OLD_FUNC(fopen);
        if((ErrFile = old_fopen(monitor_sandbox, "ab")) == NULL){
            char myErrMess[1024];
            snprintf(myErrMess, sizeof(myErrMess), "[sandbox] fopen(\"%s\")", monitor_sandbox);
            perror(myErrMess);
        }
    }
    /* NOTE: also dup2 stderr to controlling terminal */
    if (CtrTermFile == -1){
        HANDLE_OLD_FUNC(open);
        if((CtrTermFile = old_open(controllingTerminal, O_WRONLY)) == -1){
            char myErrMess[1024];
            snprintf(myErrMess, sizeof(myErrMess), "[sandbox] fopen(\"%s\")", controllingTerminal);
            perror(myErrMess);
        }
        else{
            dup2(CtrTermFile, 2);
        }
    }
}

void handleRejectedFunc(char *funcName, const char *pathName)
{
    openMonitorFile();
    if (ErrFile != NULL){
        fprintf(ErrFile, "[sandbox] %s(%s): not allowed\n", funcName, pathName);
    }
    fprintf(stderr, "[sandbox] %s(%s): not allowed\n", funcName, pathName);
    errno = EACCES;
}

void handlePermissionErr(char *funcName, const char *pathName)
{
    openMonitorFile();
    if (ErrFile != NULL){
        fprintf(ErrFile, "[sandbox] %s: access to %s is not allowed\n", funcName, pathName);
    }
    fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", funcName, pathName);
    errno = EACCES;
}

bool checkPathUnderPath(char* path, char* baseDir){
    if(strlen(baseDir) > strlen(path))
        return false;
    for(int i = 0; i < strlen(baseDir); i++)
        if (baseDir[i] != path[i])
            return false;
    return true;
}

bool checkPathUnderBaseDir(const char *pathname){
    char *baseDir, *path;
    if((baseDir = realpath(getenv("BASE_DIR"), NULL)) == NULL){
        if (ErrFile != NULL)
                fprintf(ErrFile, "[sandbox] cannot access to the base directory \"%s\"\n", getenv("BASE_DIR"));
        fprintf(stderr, "[sandbox] cannot access to the base directory \"%s\"\n", getenv("BASE_DIR"));
        exit(EXIT_FAILURE);
    }
    if((path = realpath(pathname, NULL)) == NULL){
        if (ErrFile != NULL)
                fprintf(ErrFile, "[sandbox] cannot access to %s\n", pathname);
        fprintf(stderr, "[sandbox] cannot access to %s\n", pathname);
        exit(EXIT_FAILURE);
    }
    return checkPathUnderPath(path, baseDir);
}


int chdir(const char *path){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(chdir, path);
    HANDLE_OLD_FUNC(chdir);
    EXECUTE_OLD_FUNCTION_1_PARAM(chdir, %d, path, %s);
}

int chmod(const char *pathname, mode_t mode){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(chmod, pathname);
    HANDLE_OLD_FUNC(chmod);
    EXECUTE_OLD_FUNCTION_2_PARAM(chmod, %d, pathname, %s, mode, %u);
}

int chown(const char *pathname, uid_t owner, gid_t group){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(chown, pathname);
    HANDLE_OLD_FUNC(chown);
    EXECUTE_OLD_FUNCTION_3_PARAM(chown, %d, pathname, %s, owner, %u, group, %u);
}

int creat(const char *pathname, mode_t mode){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(creat, pathname);
    HANDLE_OLD_FUNC(creat);
    EXECUTE_OLD_FUNCTION_2_PARAM(creat, %d, pathname, %s, mode, %u);
}

FILE* fopen(const char *pathname, const char *mode){
    FILE* returnedValue = NULL;
    HANDLE_PERMISSION_ERR(fopen, pathname);
    HANDLE_OLD_FUNC(fopen);
    EXECUTE_OLD_FUNCTION_2_PARAM(fopen, %p, pathname, %s, mode, %s);
}

int link(const char *path1, const char *path2){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(link, path1);
    HANDLE_PERMISSION_ERR(link, path2);
    HANDLE_OLD_FUNC(link);
    EXECUTE_OLD_FUNCTION_2_PARAM(link, %d, path1, %s, path2, %s);
}

int mkdir(const char *path, mode_t mode){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(mkdir, path);
    HANDLE_OLD_FUNC(mkdir);
    EXECUTE_OLD_FUNCTION_2_PARAM(mkdir, %d, path, %s, mode, %u);
}

int open(const char *path, int oflag, ... ){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(open, path);
    HANDLE_OLD_FUNC(open);

    if(oflag != O_CREAT){
        EXECUTE_OLD_FUNCTION_2_PARAM(open, %d, path, %s, oflag, %d);
    }
    // Open FILE with access OFLAG. If O_CREAT is in OFLAG, a third argument is the file protection.
	if(oflag == O_CREAT){
        mode_t mode = 0;
        va_list arg;
        va_start(arg, oflag);
        mode = va_arg(arg, mode_t);
        va_end (arg);
        EXECUTE_OLD_FUNCTION_3_PARAM(open, %d, path, %s, oflag, %d, mode, %u);
    }
}

int openat(int fd, const char *path, int oflag, ...){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(openat, path);
    HANDLE_OLD_FUNC(openat);

    if(oflag != O_CREAT){
        EXECUTE_OLD_FUNCTION_3_PARAM(openat, %d, fd, %d, path, %s, oflag, %d);
    }
    // Open FILE with access OFLAG. If O_CREAT is in OFLAG, a third argument is the file protection.
	if(oflag == O_CREAT){
        mode_t mode = 0;
        va_list arg;
        va_start(arg, oflag);
        mode = va_arg(arg, mode_t);
        va_end (arg);
        EXECUTE_OLD_FUNCTION_4_PARAM(openat, %d, fd, %d, path, %s, oflag, %d, mode, %u);
    }
}

DIR* opendir(const char *name){
    DIR* returnedValue = NULL; 
    HANDLE_PERMISSION_ERR(opendir, name);
    HANDLE_OLD_FUNC(opendir);
    EXECUTE_OLD_FUNCTION_1_PARAM(opendir, %p, name, %s);
}

ssize_t readlink(const char *restrict path, char *restrict buf, size_t bufsize){
    ssize_t returnedValue = -1;
    HANDLE_PERMISSION_ERR(readlink, path);
    HANDLE_OLD_FUNC(readlink);
    EXECUTE_OLD_FUNCTION_3_PARAM(readlink, %ld, path, %s, buf, %s, bufsize, %lu);
}

int remove(const char *pathname){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(remove, pathname);
    HANDLE_OLD_FUNC(remove);
    EXECUTE_OLD_FUNCTION_1_PARAM(remove, %d, pathname, %s);
}

int rmdir(const char *path){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(rmdir, path);
    HANDLE_OLD_FUNC(rmdir);
    EXECUTE_OLD_FUNCTION_1_PARAM(rmdir, %d, path, %s);
}

int __xstat(int ver, const char *path, struct stat *stat_buf){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(__xstat, path);
    HANDLE_OLD_FUNC(__xstat);

    if(old___xstat != NULL)
        returnedValue = old___xstat(ver, path, stat_buf);

    openMonitorFile();
    if (ErrFile != NULL)
        fprintf(ErrFile, "[sandbox] stat(%s, %p {st_ino=%lu, st_mode=%u, st_size=%ld, ...}) = %d\n", \
                    path, stat_buf, stat_buf->st_ino, stat_buf->st_mode, stat_buf->st_size, \
                    returnedValue);

    return returnedValue;
}

int symlink(const char *path1, const char *path2){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(symlink, path1);
    HANDLE_PERMISSION_ERR(symlink, path2);
    HANDLE_OLD_FUNC(symlink);
    EXECUTE_OLD_FUNCTION_2_PARAM(symlink, %d, path1, %s, path2, %s);
}

int unlink(const char *path){
    int returnedValue = -1;
    HANDLE_PERMISSION_ERR(unlink, path);
    HANDLE_OLD_FUNC(unlink);
    EXECUTE_OLD_FUNCTION_1_PARAM(unlink, %d, path, %s);
}


/* exec* and system parts */

int execl(const char *path, const char *arg, ...){
    handleRejectedFunc("execl", path);
    if (ErrFile != NULL)
        fclose(ErrFile);
    return -1;
}

int execle(const char *pathname, const char *arg, ...
                       /*, (char *) NULL, char *const envp[] */){
    handleRejectedFunc("execle", pathname);
    if (ErrFile != NULL)
        fclose(ErrFile);
    return -1;
}


int execlp(const char *file, const char *arg, .../* (char  *) NULL */){
    handleRejectedFunc("execlp", file);
    if (ErrFile != NULL)
        fclose(ErrFile);
    return -1;
}

int execv(const char *pathname, char *const argv[]){
    handleRejectedFunc("execv", pathname);
    if (ErrFile != NULL)
        fclose(ErrFile);
    return -1;
}

int execve(const char *filename, char *const argv[], char *const envp[]){
    handleRejectedFunc("execve", filename);
    if (ErrFile != NULL)
        fclose(ErrFile);
    return -1;
}

int execvp(const char *file, char *const argv[]){
    handleRejectedFunc("execvp", file);
    if (ErrFile != NULL)
        fclose(ErrFile);
    return -1;
}

int system(const char *command){
    handleRejectedFunc("system", command);
    if (ErrFile != NULL)
        fclose(ErrFile);
    return -1;
}
