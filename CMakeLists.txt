cmake_minimum_required(VERSION 3.16.0)

# here we specify that the project is C language only, so the default
# C compiler on the system will be used
project(ample C)
add_executable(ample main.c ast.c hash.c interpreter.c lexer.c parser.c ncl.c ssl.c dict_vars.c ampobject.c)
target_link_libraries (ample bsd)
target_compile_options (ample PRIVATE -Wall -ggdb3 -g3 -fsanitize=address,leak,undefined,pointer-compare)
target_link_options (ample PRIVATE -ggdb3 -g3 -fsanitize=address,leak,undefined,pointer-compare)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
