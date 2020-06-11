cmake_minimum_required(VERSION 2.6.0)

# here we specify that the project is C language only, so the default
# C compiler on the system will be used
project(ample C)
add_executable(ample main.c ast.c hash.c interpreter.c lexer.c parser.c ncl.c ssl.c dict_str_vars.c dict_int_vars.c)
target_link_libraries (ample bsd)
target_compile_options (ample PRIVATE -fsanitize=address)
target_link_options (ample PRIVATE -fsanitize=address)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
