# exec (xv6 user program)

## Goal

Extend the `find` program to support a `-exec cmd` flag. Instead of 
printing matching filenames, it executes `cmd file` for each file found.

Example:

$ find . wc -exec echo hi
hi ./wc

Here the command is `echo hi` and the file is `./wc`, so it runs 
`echo hi ./wc` which outputs `hi ./wc`.

## Concepts Practiced

### 1. fork

`fork` is a system call that creates a new process by:
- creating an entry in the process table
- copying the parent process content to the child

After `fork`, we have two processes running the same code:
- **parent** — continues running `find`
- **child** — will execute the command

### 2. exec

`exec` is a system call that loads a new program into the current 
process, replacing it completely.

This is why we need `fork` before `exec`:
- if we called `exec` directly, `find` itself would be destroyed
- `fork` first creates a copy of the process
- `exec` loads the new program into the child, replacing its content
- the parent blocks on `wait` until the child finishes, then continues

### 3. wait

`wait` is a system call that blocks the parent until the child 
process finishes. The parent calls `wait` after `fork` so it does 
not continue searching before the command finishes executing.

The full flow:

fork()
├── child  → exec(cmd, args)   runs the command on the file
└── parent → wait()            blocks until child finishes
                               then continues finding files
