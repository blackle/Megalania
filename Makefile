HEADERS := $(wildcard src/*.h)
SOURCES := $(wildcard src/*.c)
PROJECT := megalania

$(PROJECT) : $(HEADERS) $(SOURCES) Makefile
	gcc -o $(PROJECT) $(SOURCES) -lm -g -Wall -Werror -Wextra -O0 -flto