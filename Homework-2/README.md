# Read me - monitor function
    In this homework, we are going to practice library injection and API hijacking.
    Implementation of the sandbox as a shared library (1) (e.g., sandbox.so) and 
    then inject the shared library into a process using using your customized command 
    launcher (2) (e.g., sandbox)

## Monitored functions:
    Note: monitor_sandbox.txt is used to log information and error messages from the sandbox

    File/directory-relevant functions:
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

    System and exec* functions:
    execl
    execle
    execlp
    execv
    execve
    execvp
    system
