# exec (xv6 user program)

## Goal

Extend the `find` program to support a `-exec cmd` flag. Instead of 
printing matching filenames, it executes `cmd file` for each file found.

Example:

$ find . wc -exec echo hi
hi ./wc

Here the command is `echo hi` and the file is `./wc`, so it runs 
`echo hi ./wc` which outputs `hi ./wc`.
