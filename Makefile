# CFLAGS = -Wall -Werror -ansi -pedantic
CFLAGS = -Wall -Werror
OFILES = main.o parser.o tree.o codeblock.o headers.o paragraph.o image.o list.o node.o renderer.o iterator.o util.o

main: $(OFILES)
	gcc $(OFILES) -o md_to_tex
	rm $(OFILES)

# GENERAL FILES
main.o: src/main.c
	gcc -c src/main.c -o main.o

node.o: src/node.h src/node.c
	gcc -c src/node.c -o node.o

tree.o: src/tree.h src/tree.c
	gcc -c src/tree.c -o tree.o

## PARSER FILES
parser.o: src/parser/parser.h src/parser/parser.c
	gcc -c src/parser/parser.c -o parser.o

list.o: src/parser/features/list.h src/parser/features/list.c
	gcc -c src/parser/features/list.c -o list.o

headers.o: src/parser/features/headers.h src/parser/features/headers.c
	gcc -c src/parser/features/headers.c -o headers.o

paragraph.o: src/parser/features/paragraph.h src/parser/features/paragraph.c
	gcc -c src/parser/features/paragraph.c -o paragraph.o

codeblock.o: src/parser/features/codeblock.h src/parser/features/codeblock.c
	gcc -c src/parser/features/codeblock.c -o codeblock.o

image.o: src/parser/features/image.h src/parser/features/image.c
	gcc -c src/parser/features/image.c -o image.o

## RENDERER FILES

renderer.o: src/renderer/renderer.h src/renderer/renderer.c
	gcc -c src/renderer/renderer.c -o renderer.o

iterator.o: src/renderer/iterator.h src/renderer/iterator.c
	gcc -c src/renderer/iterator.c -o iterator.o

## UTIL FILES
util.o: src/util/util.c src/util/util.h
	gcc -c src/util/util.c -o util.o

## CLEAN FIlES
clean:
	rm $(OFILES)

