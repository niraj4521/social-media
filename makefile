# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = social_feed_engine

# Directories
SRC_DIR = src
INCLUDE_DIR = include
DATA_DIR = data

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(DATA_DIR) $(TARGET)

# Create data directory if it doesn't exist
$(DATA_DIR):
	mkdir -p $(DATA_DIR)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✅ Build successful! Run with: ./$(TARGET)"

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "🧹 Cleaned build artifacts"

# Clean everything including data
clean-all: clean
	rm -rf $(DATA_DIR)
	@echo "🧹 Cleaned all files"

# Run the program
run: all
	./$(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  make         - Build the project"
	@echo "  make run     - Build and run the project"
	@echo "  make clean   - Remove build artifacts"
	@echo "  make clean-all - Remove build artifacts and data"
	@echo "  make help    - Show this help message"

.PHONY: all clean clean-all run help