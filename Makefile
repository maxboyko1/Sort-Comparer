FLAGS = -Wall -std=c++11

all: sortcomparer

sortcomparer: sortcomparer.o
	g++ $(FLAGS) -o $@ $^

%.o: %.cc sortcomparer.h
	g++ $(FLAGS) -c $<

clean:
	rm -f sortcomparer *.o
