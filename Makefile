OBJ = main.o
INC = -I "./"

Pismriver: $(OBJ)
	g++ $(OBJ) -fvisibility=hidden -o prismriver
	rm -f $(OBJ)

main.o:
	g++ -fvisibility=hidden -c main.cpp $(INC)

clean:
	rm -f $(OBJ)
