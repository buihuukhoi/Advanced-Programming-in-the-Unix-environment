# Read me - monitor function
    In this homework, we are going to practice library injection and API hijacking. Implementation of the sandbox as a shared library (1) (e.g., sandbox.so) and then inject the shared library into a process using using your customized command launcher (2) (e.g., sandbox)

## Monitored functions:

## file/directory-relevant functions
    chdir
    chmod
    chown
    creat
    fopen
    link
    mkdir
    open
    openat
    opendir
    readlink
    remove
    rename
    rmdir
    stat
    symlink
    unlink

## system and exec* functions
    execl
    execle
    execlp
    execv
    execve
    execvp
    system
