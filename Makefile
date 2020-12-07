GCC=gcc
EXEC=tsp
EXEC_VISUAL=tsp_visual

tsp:
	$(GCC) -O3 -std=c99 tsp.c -o $(EXEC)

tsp_visual:
	$(GCC) -O3 -std=c99 `pkg-config --libs --cflags raylib` tsp_visual.c -o $(EXEC_VISUAL)

clean:
	rm -f $(EXEC) $(EXEC_VISUAL)
