# Usage
LD_PRELOAD=./sandbox.so ./sandbox `your_commands`
    
    your_commands examples:
        ls
        -- ls -a
        -- ls /
        -- ls -la / Makefile
        -- ls -la / Makefile >/dev/null
        -- sh -c 'ls'
        
    LD_PRELOAD is used to inject library

# Read me - monitor function
    In this homework, we are going to practice library injection and API hijacking.
    Implementation of the sandbox as a shared library (1) (e.g., sandbox.so) and 
    then inject the shared library into a process using using your customized command 
    launcher (2) (e.g., sandbox)

## Monitored functions:
    Note:   - monitor_sandbox.txt is used to log information and error messages from the sandbox
            - If an access to an invalid path is detected or the use of "system or exec*" is rejected,
                the functions will return -1 (if int) or NULL (if pointer) and the errno will be set
                to EACCES (permission denied).

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
