.PHONY: all clean

all: todo

todo:
	nasm -f elf64 -g -F dwarf main.asm -o main.o
	ld -o main main.o

clean:
	rm main
	rm *.o
