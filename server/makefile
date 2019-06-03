# This makefile was based on
# https://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-for-gcc-on-linux

TARGET = main
CC = gcc
CFLAGS = -g -std=c99 -O3

.PHONY: default all clean

default: clear $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@	

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall -O2 -o $@	
	-rm -f *.o
	
	-pwd

run: default
	- sudo ./main 1

run_tree: default
	- sudo ./main 1

run_hypercube: default
	- sudo ./main 2

run_torus: default
	- sudo ./main 3

clean:
	-rm -f *.o
	-rm -f $(TARGET)

clear:
	-pwd
