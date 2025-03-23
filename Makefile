build:
	gcc -Wall -std=c99 ./src/*.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -o game

run:
	./game

clean:
	rm game