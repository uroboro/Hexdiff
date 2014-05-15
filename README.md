Hexdiff
=======

CLI tool for viewing differences between two similar files.

It's default interface is two columns of hexadecimal values, one for each file, with the starting offset of each line.
It displays differences by highlighting the bytes with colors.

Other uses of this tool include returning the number of differences between files and saving a range of differences to a new file.

It supports multiple ranges for bytes and differences.

To-do:

- use a structure to pass data around instead of numerous args?
- find a workaround for global variables
