main : main.c substring_enumerator.c Makefile
	gcc -o main main.c substring_enumerator.c -g -Wall -Werror -Wextra -O2 -flto