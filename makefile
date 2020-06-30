all:
	clang -g -Wall -Wextra -fsanitize=address -std=c89 -pedantic -Wno-switch build.c -o ample-clang

