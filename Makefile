CC = gcc
CFLAGS = -DDEBUG -w
@ := @

.PHONY: all build

run: build
	@build/mlp

build:
	@mkdir -p build/
	@$(CC) -o build/mlp $(CFLAGS) src/*.c
