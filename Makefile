# Write the Game Boy ROM of your choice here.
ROM = test/tetris.gb

#Compilation parameters
FILES = main.cpp CPU.cpp MMU.cpp operations.cpp cpuexec.cpp window.cpp PPU.cpp timers.cpp 
FLAGS = -o jasmine -lSDL2 -lSDL2main
WFLAGS = -I. -ggdb -Wall -Wextra -Werror

all:
	g++ $(FILES) $(FLAGS)
	sh makebios.sh

warning:
	g++ $(FILES) $(FLAGS) $(WFLAGS)

test: 
	./jasmine $(ROM)

clean: 
	rm -f jasmine

clear:
	rm -f jasmine
	clear