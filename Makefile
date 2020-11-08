CC = gcc
RM = rm -rf

TARGET = fractalascii

SRCS += Source/main.c
SRCS += Source/fractalascii.c
SRCS += Source/mandelbrot.c
SRCS += Source/screen.c
SRCS += Source/camera.c
SRCS += Source/stats.c
SRCS += Source/timer.c

INCLUDES = Include/

FLAGS = -lcurses -Wall -g $(foreach d, $(INCLUDES), -I$d)

.PHONY: $(TARGET)
all: $(TARGET) run
	
$(TARGET):
	rm -rf .Build/
	mkdir .Build/
	$(CC) $(SRCS) $(FLAGS) -o .Build/$(TARGET)

run:
	clear
	.Build/$(TARGET)
