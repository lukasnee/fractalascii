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

FLAGS = -lcurses -Wall $(foreach d, $(INCLUDES), -I$d)

.PHONY: clean
clean:
	rm -rf .Build/
	mkdir .Build/

.PHONY: $(TARGET)
$(TARGET): clean
	mkdir .Build/Release/ 
	$(CC) $(SRCS) $(FLAGS) -o .Build/Release/$(TARGET)

.PHONY: $(TARGET).debug
$(TARGET).debug: clean
	mkdir .Build/Debug/ 
	$(CC) $(SRCS) $(FLAGS) -g -o .Build/Debug/$(TARGET)

.PHONY: run
run: $(TARGET)
	clear
	.Build/Release/$(TARGET)

.PHONY: run.debug
run.debug: $(TARGET).debug
	clear
	.Build/Debug/$(TARGET)

.PHONY: all
all: run
