snake: main.o
	g++ main.o -o snake -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	g++ -c main.cpp -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm *.o snake
