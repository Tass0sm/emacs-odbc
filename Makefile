CC      = gcc
LD      = gcc
CFLAGS  = -ggdb3 -Wall
LDFLAGS = -lodbc

all: emacs-odbc.so

# make shared library out of the object file
%.so: emacs-odbc.o utils.o
	$(LD) -shared $(LDFLAGS) -o $@ $^

# compile source file to object file
%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $<

clean:
	rm -f *.so *.o *.dylib *.dll
