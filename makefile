all:
	clang -g -Wall -Wextra -pedantic -fsanitize=address -std=gnu11 -Wno-switch build.c -o ample-clang

