CXXFLAGS+=-std=c++11 -Wall -g
LDFLAGS=-ldl -lSDL2

EXECUTABLE=game
OBJECTS=game.o main.o model.o map.o character.o

all: test-game $(EXECUTABLE)

test-game: test.o glad/glad.c
	$(CXX) -o test-game $(LDFLAGS) test.o glad/glad.c

test.o: test.cpp
	$(CXX) -c $(CXXFLAGS) test.cpp

game: $(OBJECTS) glad/glad.c
	$(CXX) -o $(EXECUTABLE) $(LDFLAGS) $(OBJECTS) glad/glad.c

%.o: %.cpp %.hpp
	$(CXX) -c $(CXXFLAGS) $<

clean:
	rm $(EXECUTABLE) $(OBJECTS)
