CC=gcc


SRCS += main.c
SRCS += mandelbrot.c
SRCS += screen.c
SRCS += camera.c
SRCS += stats.c
SRCS += timer.c

FLAGS = -lcurses -Wall -g

TARGET=fractalascii

.PHONY: $(TARGET)

all: $(TARGET) run
	
$(TARGET):
	$(CC) $(SRCS) $(FLAGS) -o $(TARGET)

run:
	clear
	./$(TARGET)
