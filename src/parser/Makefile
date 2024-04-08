parser: parser.o tree.o codeblock.o headers.o paragraph.o node.o
	gcc parser.o tree.o codeblock.o headers.o paragraph.o node.o -o parser

parser.o: parser.c
	gcc -c parser.c -o parser.o

node.o: node.c
	gcc -c node.c -o node.o

tree.o: tree.c
	gcc -c tree.c -o tree.o

headers.o: features/headers.c
	gcc -c features/headers.c -o headers.o

paragraph.o: features/paragraph.c
	gcc -c features/paragraph.c -o paragraph.o

codeblock.o: features/codeblock.c
	gcc -c features/codeblock.c -o codeblock.o

clean:
	rm *.o
