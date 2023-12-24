all: empty-module.c
	gcc -shared empty-module.c -o empty-module.so
