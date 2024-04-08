# CFLAGS = -Wall -Werror -ansi -pedantic
CFLAGS =
OFILES = main.o parser.o tree.o codeblock.o headers.o paragraph.o node.o

main: $(OFILES)
	gcc $(OFILES) -o md_to_tex
	rm *.o

## PARSER FILES
parser.o: parser/parser.c
	gcc -c parser/parser.c -o parser.o

node.o: parser/node.c
	gcc -c parser/node.c -o node.o

tree.o: parser/tree.c
	gcc -c parser/tree.c -o tree.o

headers.o: parser/features/headers.c
	gcc -c parser/features/headers.c -o headers.o

paragraph.o: parser/features/paragraph.c
	gcc -c parser/features/paragraph.c -o paragraph.o

codeblock.o: parser/features/codeblock.c
	gcc -c parser/features/codeblock.c -o codeblock.o

clean:
	rm *.o
