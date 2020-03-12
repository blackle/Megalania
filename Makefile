HEADERS := substring_enumerator.h memory_mapper.h
SOURCES := main.c substring_enumerator.c memory_mapper.c
PROJECT := megalania

$(PROJECT) : $(HEADERS) $(SOURCES) Makefile
	gcc -o $(PROJECT) $(SOURCES) -g -Wall -Werror -Wextra -O2 -flto