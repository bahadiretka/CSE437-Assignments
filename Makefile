# Makefile
CXX=g++
CXXFLAGS=-std=c++14 -Wall -Wextra

all: timer

timer: Timer.o main.o
	$(CXX) $(CXXFLAGS) -o timer Timer.o main.o

Timer.o: Timer.cpp Timer.h
	$(CXX) $(CXXFLAGS) -c Timer.cpp

main.o: main.cpp Timer.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f *.o timer
