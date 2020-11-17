# Makefile v1.0.0

all: compile

run: myshell.o
	./myshell

compile: myshell.c
	gcc myshell.c libparser.a -static -o myshell

clean:
	rm myshell