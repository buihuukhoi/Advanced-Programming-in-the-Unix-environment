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
#define sandbox_monitor "sandbox_monitor.txt"

static int (*old_chdir)(const char *path) = NULL;
static int (*old_chmod)(const char *pathname, mode_t mode) = NULL;
static int (*old_chown)(const char *pathname, uid_t owner, gid_t group) = NULL;
static int (*old_creat)(const char *pathname, mode_t mode) = NULL;
static FILE* (*old_fopen)(const char *pathname, const char *mode) = NULL;
static int (*old_link)(const char *path1, const char *path2) = NULL;
static int (*old_mkdir)(const char *path, mode_t mode) = NULL;
static int (*old_execv)(const char *pathname, char *const argv[]) = NULL;
static DIR* (*old_opendir)(const char *name) = NULL;
static int (*old___xstat)(int ver, const char * path, struct stat * stat_buf) = NULL;

ino_t pathInode = -1;
FILE* ErrFile = NULL;


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

#define OPEN_MONITOR_FILE()\
    if (ErrFile == NULL){\
        if((ErrFile = old_fopen(sandbox_monitor, "ab")) == NULL){\
            char myErrMess[1024];\
            snprintf(myErrMess, sizeof(myErrMess), "[sandbox] fopen(\"%s\")", sandbox_monitor);\
            perror(myErrMess);\
        }\
    }
    
#define EXECUTE_OLD_FUNCTION_1_PARAM(name, formatRetStr, param1, formatStr1)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1);\
    fprintf(ErrFile, "[sandbox] " #name "(" #formatStr1 ") = " #formatRetStr "\n", param1, returnedValue);\
    return returnedValue;
    
#define EXECUTE_OLD_FUNCTION_2_PARAM(name, formatRetStr, param1, formatStr1, param2, formatStr2)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1, param2);\
    fprintf(ErrFile, "[sandbox] " #name "(" #formatStr1 ", " #formatStr2 ") = " #formatRetStr "\n", param1, param2, returnedValue);\
    return returnedValue;

#define EXECUTE_OLD_FUNCTION_3_PARAM(name, formatRetStr, param1, formatStr1, param2, formatStr2, param3, formatStr3)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1, param2, param3);\
    fprintf(ErrFile, "[sandbox] " #name "(" #formatStr1 ", " #formatStr2 ", " #formatStr3 ") = " #formatRetStr "\n", param1, param2, param3, returnedValue);\
    return returnedValue;


__attribute__((constructor)) static void beforeMain(){
    HANDLE_OLD_FUNC(fopen);
    OPEN_MONITOR_FILE();
}

void handlePermissionErr(char *funcName, const char *pathname)
{
    char myErrMess[1024];
    snprintf(myErrMess, sizeof(myErrMess), "[sandbox] %s: access to %s is not allowed\n", funcName, pathname);
    if (ErrFile != NULL){
        fprintf(ErrFile, myErrMess, NULL);
    }
    fprintf(stderr, myErrMess, NULL);
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
    HANDLE_OLD_FUNC(chdir);

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

/*
int chdir(const char *path){
    int returnedValue = -1;
    HANDLE_OLD_FUNC(chdir);
    EXECUTE_OLD_FUNCTION_1_PARAM(chdir, %d, path, %s);
}

int chmod(const char *pathname, mode_t mode){
    int returnedValue = 
    HANDLE_OLD_FUNC(chmod, int, 0);
    EXECUTE_OLD_FUNCTION_2_PARAM(chmod, %d, pathname, %s, mode, %d);
}


int chown(const char *pathname, uid_t owner, gid_t group){
    int returnedValue = 
    HANDLE_OLD_FUNC(chown, int, 0);
    EXECUTE_OLD_FUNCTION_3_PARAM(chown, %d, pathname, %s, owner, %d, group, %d);
}

int creat(const char *pathname, mode_t mode){
    int returnedValue = 
    HANDLE_OLD_FUNC(creat, int, 0);
    EXECUTE_OLD_FUNCTION_2_PARAM(creat, %d, pathname, %s, mode, %d);
}
*/

FILE* fopen(const char *pathname, const char *mode){
    FILE* returnedValue = NULL;
    HANDLE_PERMISSION_ERR(fopen, pathname);
    HANDLE_OLD_FUNC(fopen);
    EXECUTE_OLD_FUNCTION_2_PARAM(fopen, %p, pathname, %s, mode, %s);
}

/*
int link(const char *path1, const char *path2){
    int returnedValue = 
    HANDLE_OLD_FUNC(link, int, 0);
    EXECUTE_OLD_FUNCTION_2_PARAM(link, %d, path1, %s, path2, %s);
}

int mkdir(const char *path, mode_t mode){
    int returnedValue = 
    HANDLE_OLD_FUNC(mkdir, int, 0);
    EXECUTE_OLD_FUNCTION_2_PARAM(mkdir, %d, path, %s, mode, %d);
}

int execv(const char *pathname, char *const argv[]){
    int returnedValue = 
    HANDLE_OLD_FUNC(execv, int, 0);
    fprintf(stderr, "[sandbox] we are in the shared library!!!\n");

    EXECUTE_OLD_FUNCTION_2_PARAM(execv, %d, pathname, %s, argv, %p);
}
*/

DIR* opendir(const char *name){
    DIR* returnedValue = NULL; 
    HANDLE_PERMISSION_ERR(opendir, name);
    HANDLE_OLD_FUNC(opendir);
    EXECUTE_OLD_FUNCTION_1_PARAM(opendir, %p, name, %s);
}

/*
int execl(const char *file, const char *args, ...){
    va_list ap;
    char *array[31 +1];
    int argno = 0;

    fprintf(stderr, "[sandbox] we are in the shared library!!!\n");

    va_start(ap, args);
    while (args != 0 && argno < 31)
    {
        array[argno++] = args;
        args = va_arg(ap, const char *);
    }
    array[argno] = (char *) 0;
    va_end(ap);
    return execv(file, array);
}


FILE* fopen(const char *pathname, const char *mode){
    if(old_fopen == NULL){
        void *handle = dlopen("libc.so.6", RTLD_LAZY);
        if(handle != NULL){
            old_fopen = dlsym(handle, "fopen");
        }
    }
    FILE* returnedValue = NULL;
    if(old_fopen != NULL)
        returnedValue = old_fopen(pathname, mode);
    else
        fprintf(stderr, "fopen failed");
    fprintf(stderr, "fopen(\"%s\", \"%s\") = %p\n", pathname, mode, returnedValue);
    
    return returnedValue;
}
*/
/*
int chdir(const char *path){
    if(old_chdir == NULL){
        void *handle = dlopen("libc.so.6", RTLD_LAZY);
        if(handle != NULL){
            old_chdir = dlsym(handle, "chdir");
        }
    }
    int returnedValue = 0;
    if(old_chdir != NULL)
        returnedValue = old_chdir(path);
    else
        fprintf(stderr, "chdir failed");
    fprintf(stderr, "chdir(\"%s\") = %d\n", path, returnedValue);
    
    return returnedValue;
}
*/