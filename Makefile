CC=gcc
CFLAGS=-O3 -Wall -Wextra -std=gnu99

OUT_FOLDER=bin
IN_FOLDER=source

all: bin
	$(CC) $(IN_FOLDER)/rt_timer.c -o $(OUT_FOLDER)/rt_timer $(CFLAGS)

bin:
	mkdir bin

purge:
	rm bin/*
