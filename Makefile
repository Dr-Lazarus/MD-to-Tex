# CFLAGS = -Wall -Werror -ansi -pedantic
CFLAGS =
OFILES = main.o parser.o tree.o codeblock.o headers.o paragraph.o node.o renderer.o

main: $(OFILES)
	gcc $(OFILES) -o md_to_tex
	rm $(OFILES)

# GENERAL FILES
main.o: src/main.c
	gcc -c src/main.c -o main.o

node.o: src/node.h src/node.c
	gcc -c src/node.c -o node.o

## PARSER FILES
parser.o: src/parser/parser.h src/parser/parser.c
	gcc -c src/parser/parser.c -o parser.o

tree.o: src/parser/tree.h src/parser/tree.c
	gcc -c src/parser/tree.c -o tree.o

headers.o: src/parser/features/headers.h src/parser/features/headers.c
	gcc -c src/parser/features/headers.c -o headers.o

paragraph.o: src/parser/features/paragraph.h src/parser/features/paragraph.c
	gcc -c src/parser/features/paragraph.c -o paragraph.o

codeblock.o: src/parser/features/codeblock.h src/parser/features/codeblock.c
	gcc -c src/parser/features/codeblock.c -o codeblock.o

## RENDERER FILES

renderer.o: src/renderer/renderer.h src/renderer/renderer.c
	gcc -c src/renderer/renderer.c -o renderer.o

## CLEAN FIlES
clean:
	rm *.o

