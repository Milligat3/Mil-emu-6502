# Mil-emu-6502
This is my take on emulating 6502. Does not include assembler - reads only .bin files.
To compile program for it, you need to use cl65
Example:
> cl65 -t none Example.s -o Example.bin --start-addr 0x600

# Not implemented or not done(But planned):

> Illegal opcodes

> PPU, APU emulation

> Cycle counting

> Refactor whole thing to make opcodes in header, their implementation in .c file

> Use write_mem/read_mem instead of direct memory[addr]

But for now, I am happy with what I have. And if you want, you can try to use it for educational purposes.
By the way, it passed Klaus Dormann functional test. But I had to modify - load rom from 0x0000 (You can change it in source code)

To build it simply(you need raylib):
> gcc (path to raylib)\src\raylib.rc.data -s -static -O2 -std=c99 -Wall -I(path to raylib)\src -Iexternal -lraylib -lopengl32 -lgdi32 -lwinmm

It is a temporary solution, will make a makefile for it in future.
