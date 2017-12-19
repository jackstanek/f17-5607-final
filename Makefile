CXXFLAGS+=-O3 -std=c++11 -Wall -g
LDFLAGS=-ldl -lSDL2

EXECUTABLE=game
OBJECTS=game.o main.o model.o map.o character.o framebuffer.o dungeon.o

game: $(OBJECTS) glad/glad.c
	$(CXX) -o $(EXECUTABLE) $(OBJECTS) glad/glad.c $(LDFLAGS)

%.o: %.cpp %.hpp
	$(CXX) -c $(CXXFLAGS) $<

clean:
	rm $(EXECUTABLE) *.o
