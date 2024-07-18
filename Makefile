all: clean build

clean:
	rm -f ./build

build:
	gcc transpiler.c socket.c probe.c -o build