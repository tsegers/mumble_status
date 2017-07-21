# ----------------------------------------------------
# Default makefile
# ----------------------------------------------------
TARGET  = mumble_status

# Init
SHELL = /bin/sh
CC = gcc

# Flags
FLAGS   		= -std=gnu99 $(INCLUDE_DIRS)
CFLAGS 			= -W -g -O2
LDFLAGS 		=
LIBS 			=
INCLUDE_DIRS 	= -Iinclude -Isrc

# Files
SOURCES = $(wildcard ./*.c)
HEADERS = $(wildcard ./*.h)
OBJECTS = $(SOURCES:.c=.o)
MANUAL = readme.md
EXTRA_DIST = 

# Default
default: clean compile

# Compile
compile: $(TARGET) clean

# Doc
doc: clean
	doxygen Doxyfile

# Clean
clean:
	rm -f $(OBJECTS)

install: compile
	cp ./$(TARGET) /usr/bin

uninstall:
	rm /usr/bin/$(TARGET)


# Options
options:
	@echo "$(TARGET) build options:"
	@echo "FLAGS        = ${FLAGS}"
	@echo "CFLAGS       = ${CFLAGS}"
	@echo "LDFLAGS      = ${LDFLAGS}"
	@echo "DEBUGFLAGS   = ${DEBUGFLAGS}"
	@echo "RELEASEFLAGS = ${RELEASEFLAGS}"
	@echo "INCLUDE_DIRS = ${INCLUDE_DIRS}"
	@echo "EXTRA_DIST   = ${EXTRA_DIST}"

# Target
$(TARGET): $(OBJECTS) 
	$(CC) $(FLAGS) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Objects
%.o: %.c $(HEADERS)
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $< $(LDFLAGS)




