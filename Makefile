
all:
	set -e
	gcc mandelbrot.c -o mandelbrot
	./mandelbrot
