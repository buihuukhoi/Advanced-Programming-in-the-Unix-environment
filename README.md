# Advanced-Programming-in-the-Unix-environment

1. `netstat` Implementation of a 'netstat -nap' tool to list all the existing TCP and UDP connections. For each identified connection (socket descriptor), find the corresponding process name and its command lines that creates the connection (socket descriptor).
2. `sandbox` Implementation of the sandbox as a shared library (1) (e.g., sandbox.so) and then inject the shared library into a process using the customized command launcher (2) (e.g., sandbox).
3. `assembly` small assembly exercises.
4. `ptrace_applications` ptrace's applications. Ptrace is a system call which allows one process (tracer) to observe and control the execution of another process (tracee), examine and change the tracee's memory and registers.
