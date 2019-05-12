# This makefile was based on
# https://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-for-gcc-on-linux

TARGET = main
CC = gcc
CFLAGS = -g -std=c99 -O3

.PHONY: default all clean

default: clear
	- gcc -Wall -O2 main.c -o prog

run: default
	-./prog 

clean:
	-rm -f *.o
	-rm -f prog

clear:
	-clear