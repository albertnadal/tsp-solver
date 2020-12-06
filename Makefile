build:
	gcc -std=c99 `pkg-config --libs --cflags raylib` tsp.c -o tsp