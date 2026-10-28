all:
	g++ main.cpp -o juego -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

run: all
	./juego

clean:
	rm -f juego