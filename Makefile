CXX=clang++
DEBUG=-g
OBJS=Emulator.o Main.o
CXXFLAGS=-Wall -Wextra -pedantic -std=c++14 $(DEBUG)

all: $(OBJS)
	$(CXX) $(OBJS) -o bin/emu

run: $(OBJS)
	$(CXX) $(OBJS) -o bin/emu
	./bin/emu

Emulator.o: Emulator.hpp Emulator.cpp
	$(CXX) Emulator.hpp Emulator.cpp $(CXXFLAGS) -c

Main.o: Main.cpp
	$(CXX) Main.cpp $(CXXFLAGS) -c

clean:
	rm -rf *.o
	rm -rf bin/emu