TARGETS=ringmaster player

all: $(TARGETS)
clean:
	rm -f $(TARGETS) *~

ringmaster: ringmaster.cpp potato.cpp potato.hpp
	g++ -g -o ringmaster ringmaster.cpp potato.cpp

player: player.cpp potato.cpp potato.hpp
	g++ -g -o player player.cpp potato.cpp 
