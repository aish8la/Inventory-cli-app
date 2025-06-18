# Compiler and flags
CC=gcc
CFLAGS=-I./include -Wall -g

# Directories
SRCDIR=src
INCDIR=include
DISTDIR=dist
OBJDIR=obj

# Source files and objects
SOURCES=$(wildcard $(SRCDIR)/*.c) main.c
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJECTS:=$(OBJECTS:main.c=$(OBJDIR)/main.o)

# Target executable
TARGET=$(DISTDIR)/program.exe

# Default target
all: $(TARGET)

# Create target executable in dist folder
$(TARGET): $(OBJECTS) | $(DISTDIR)
	$(CC) -o $@ $(OBJECTS)
	@echo "Build complete! Executable created at $(TARGET)"

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o: main.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if they don't exist
$(DISTDIR):
	mkdir -p $(DISTDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(DISTDIR)
	@echo "Clean complete!"

# Clean and rebuild
rebuild: clean all

# Install/copy to another location (optional)
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