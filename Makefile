CC = clang

compile: main.c
	$(CC) main.c libraylib.a -lm `sdl2-config --libs --cflags` -o main

run: compile
	./main

