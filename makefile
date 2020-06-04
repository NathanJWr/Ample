DIRS=bin bin/debug bin/release obj obj/debug obj/release
$(shell mkdir -p $(DIRS))

ifneq ($(BUILD),release)
    BUILD = debug
endif

EXEDIR      = bin/$(BUILD)/
OBJDIR      = obj/$(BUILD)/

EXE         = $(EXEDIR)tl
SOURCES     = $(wildcard *.c)
OBJECTS     = $(addprefix $(OBJDIR), $(SOURCES:.c=.o))

CC = gcc
LD = ld

CFLAGS = -Wall
LDFLAGS = -lbsd
ifeq ($(BUILD), debug)
	CFLAGS+=-fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope 
	LDFLAGS+=-fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope 
endif
ifeq ($(BUILD), release)
	CFLAGS+=-O2
endif




all : $(EXE)

.PHONY : release debug    
release debug :
	$(MAKE) BUILD=$@

$(EXE) : $(OBJECTS)
	$(CC) $(CFLAGS)  -o $@ $^ $(LDFLAGS)

$(OBJDIR)%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf bin obj