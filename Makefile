HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.c)
PROJECT := megalania

$(PROJECT) : $(HEADERS) $(SOURCES) Makefile
	gcc -o $(PROJECT) $(SOURCES) -lm -g -Wall -Werror -Wextra -O2 -flto