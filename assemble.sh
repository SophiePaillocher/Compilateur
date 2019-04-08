#!/bin/bash

execname=`basename -s .nasm $1`
dirname=`dirname $1`

nasm -f elf -dwarf -g $1
ld -m elf_i386 -o ${dirname}/${execname} ${dirname}/${execname}.o
rm ${dirname}/${execname}.o
