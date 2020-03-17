HEADERS := substring_enumerator.h memory_mapper.h probability.h probability_model.h encoder_interface.h
SOURCES := main.c substring_enumerator.c memory_mapper.c probability_model.c
PROJECT := megalania

$(PROJECT) : $(HEADERS) $(SOURCES) Makefile
	gcc -o $(PROJECT) $(SOURCES) -g -Wall -Werror -Wextra -O2 -flto