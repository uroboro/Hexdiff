Hexdiff
=======

CLI tool for viewing differences between two similar files

It's default interface is two columns of hexadecimal values, one for each file, with the starting offset of each line.
It displays differences by highlighting the bytes with colors.

Other uses of this tool include returning the number of differences between files and saving to disk one file for each difference*.

It supports basic* ranges for bytes and differences.

To-do:

- add single argument range (like 1:10 or 1-10)
- add single output file
- support multiple ranges

--------------------------------------
* will be changed in a future update
