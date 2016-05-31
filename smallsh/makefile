all: smallsh

smallsh.o: smallsh.c smallsh.h
	gcc -c smallsh.c

#getLine.o: getLine.c smallsh.h
#	gcc -c getLine.c

splitLine.o: splitLine.c smallsh.h
	gcc -c splitLine.c

execute.o: execute.c smallsh.h
	gcc -c execute.c

smallsh: smallsh.o splitLine.o execute.o
	gcc -o smallsh -lm smallsh.o splitLine.o execute.o

clean:
	rm smallsh.o
	rm splitLine.o
	rm execute.o
	rm smallsh