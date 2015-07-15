# ----------------------------------------------------
# Default makefile
# ----------------------------------------------------
TARGET  = mumble_status

# Init
SHELL = /bin/sh
CC = gcc

# Flags
FLAGS   		= -std=c99 $(INCLUDE_DIRS)
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
default: compile

# Compile
compile: $(TARGET) clean

# Doc
doc: clean
	doxygen Doxyfile

# Run
run: compile
	./$(TARGET)

# Clean
clean:
	rm -f $(OBJECTS)

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
	$(CC) $(FLAGS) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

# Objects
%.o: %.c $(HEADERS)
	$(CC) $(FLAGS) $(CFLAGS) $(LDFLAGS) -c -o $@ $<




