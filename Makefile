COMPILER = gcc
COMPILERFLAGS = -Wall -std=c99
LDFLAGS = -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
SOURCES = ./src/main.c ./src/player.c ./src/renderer.c ./src/mechanics.c
VERSION = v0.9
EXECUTABLE = Fcubed_$(VERSION).exe

build: resource.o
	$(COMPILER) $(COMPILERFLAGS) $(SOURCES) resource.o $(LDFLAGS) -o $(EXECUTABLE)

resource.o: src/resource.rc
	windres src/resource.rc -O coff -o resource.o

run:
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) resource.o