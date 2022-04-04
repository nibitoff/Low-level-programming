flags = felf64

all: main.o lib.o dict.o
	ld -o myProgram main.o dict.o lib.o

main.o: main.asm colon.inc words.inc
	nasm -$(flags)  main.asm

dict.o: dict.asm 
	nasm -$(flags) dict.asm


lib.o: lib.asm 
	nasm -$(flags) lib.asm

clean:
	rm *.o all
