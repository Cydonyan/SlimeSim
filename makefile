build: main.cpp
	rm -f main.o
	g++ -c main.cpp
	rm -f app
	g++ main.o -o app -lsfml-graphics -lsfml-window -lsfml-system
run: build
	./app