# CFLAGS = -Wall -Werror -ansi -pedantic
CFLAGS =
OFILES = main.o parser.o tree.o codeblock.o headers.o paragraph.o node.o

main: $(OFILES)
	gcc $(OFILES) -o md_to_tex
	rm $(OFILES)

main.o: src/main.c
	gcc -c src/main.c -o main.o

## PARSER FILES
parser.o: src/parser/parser.c
	gcc -c src/parser/parser.c -o parser.o

node.o: src/parser/node.c
	gcc -c src/parser/node.c -o node.o

tree.o: src/parser/tree.c
	gcc -c src/parser/tree.c -o tree.o

headers.o: src/parser/features/headers.c
	gcc -c src/parser/features/headers.c -o headers.o

paragraph.o: src/parser/features/paragraph.c
	gcc -c src/parser/features/paragraph.c -o paragraph.o

codeblock.o: src/parser/features/codeblock.c
	gcc -c src/parser/features/codeblock.c -o codeblock.o

clean:
	rm *.o

