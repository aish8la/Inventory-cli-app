# Compiler and flags
CC=gcc
CFLAGS=-I./include -I./external/include -Wall -g -Wno-unused-but-set-variable

# Directories
SRCDIR=src
INCDIR=include
DISTDIR=dist
OBJDIR=obj

EXTSRCDIR=external/src
EXTINCDIR=external/include
EXTOBJDIR=$(OBJDIR)/external

# Source files
SOURCES=$(wildcard $(SRCDIR)/*.c) main.c
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJECTS:=$(OBJECTS:main.c=$(OBJDIR)/main.o)

# External sources and objects
EXTSOURCES=$(wildcard $(EXTSRCDIR)/*.c)
EXTOBJECTS=$(EXTSOURCES:$(EXTSRCDIR)/%.c=$(EXTOBJDIR)/%.o)

# Target executable
TARGET=$(DISTDIR)/program.exe

# Default target
all: $(TARGET)

# Create target executable
$(TARGET): $(OBJECTS) $(EXTOBJECTS) | $(DISTDIR)
	$(CC) -o $@ $(OBJECTS) $(EXTOBJECTS)
	@echo "Build complete! Executable created at $(TARGET)"

# Compile internal source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o: main.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile external source files
$(EXTOBJDIR)/%.o: $(EXTSRCDIR)/%.c | $(EXTOBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(DISTDIR):
	mkdir -p $(DISTDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXTOBJDIR):
	mkdir -p $(EXTOBJDIR)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(DISTDIR)
	@echo "Clean complete!"

# Clean and rebuild
rebuild: clean all

# Install/copy to another location
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Run the program
run: $(TARGET)
	cd $(DISTDIR) && ./program.exe

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the program (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  rebuild  - Clean and build"
	@echo "  run      - Build and run the program"
	@echo "  install  - Install to system path"
	@echo "  help     - Show this help"

# Declare phony targets
.PHONY: all clean rebuild install run help