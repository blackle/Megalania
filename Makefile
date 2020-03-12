HEADERS := substring_enumerator.h memory_mapper.h
SOURCES := main.c substring_enumerator.c memory_mapper.c

main : $(HEADERS) $(SOURCES) Makefile
	gcc -o main $(SOURCES) -g -Wall -Werror -Wextra -O2 -flto