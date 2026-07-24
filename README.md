# xv6 Labs Reference

This repository contains notes and explanations of the xv6 labs I solved, organized for learning purposes.

## Labs

1. **sleep (easy)**
   - User-level program to pause for a specified number of ticks.
   - Learn about system calls, command-line arguments, and user programs in xv6.

2. **sixfive (moderate)**
   - Reads numbers from files and prints multiples of 5 or 6.
   - Learn about file I/O, string parsing, and separator handling in user programs.

3. **memdump (easy)**
   - Implements a function that reads raw memory and prints it according to a format string.
   - Learn about C pointers, pointer casting, and how to interpret raw bytes as different types.

4. **find (moderate)**
   - Implements a simple version of the UNIX `find` program that searches a directory tree for files with a specific name.
   - Learn about directory traversal, recursion, system calls (`open`, `read`, `fstat`).

5. **exec (moderate)**
   - Extends the `find` program to execute a command on each matched file instead of printing it.
   - Learn about process creation with `fork`, program execution with `exec`, and synchronization with `wait`.
