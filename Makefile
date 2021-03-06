HDR =	wiz.h piz.h ast.h analyze.h std.h missing.h symbol.h codegen.h

OBJ =	wiz.o piz.o liz.o ast.o analyze.o std.o symbol.o codegen.o

CC = 	gcc -Wall -w

wiz: $(OBJ)
	$(CC) -o wiz $(OBJ)

piz.c piz.h: piz.y ast.h std.h missing.h
	bison --debug -v -d piz.y -o piz.c

liz.c: liz.l piz.h std.h ast.h
	flex -s -oliz.c liz.l

clean:
	/bin/rm $(OBJ) piz.c piz.h piz.output liz.c wiz

$(OBJ):	$(HDR)

