SOURCES = main.c

all: ${SOURCES}
	cc ${SOURCES} -lX11

run: all
	./a.out
