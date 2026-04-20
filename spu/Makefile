FLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE
NODEBUG = -g
all: stack

stack: stack_main.o dump_functions.o stack_functions.o
	g++ $(FLAGS) stack_main.o dump_functions.o stack_functions.o -o stack

stack_main.o: stack_main.cpp
	g++ $(FLAGS) -c stack_main.cpp

dump_functions.o: dump_functions.cpp
	g++ $(FLAGS) -c dump_functions.cpp

stack_functions.o: stack_functions.cpp
	g++ $(FLAGS) -c stack_functions.cpp

clean:
	rm -rf *.o stack
