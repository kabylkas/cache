# Define the compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -O2 -std=c++11

# Define the target executable
TARGET := main

# Define the directories
SRC_DIR := source
OBJ_DIR := obj

# Add the source directory to the include path
INCLUDES := -I$(SRC_DIR)

# Define the object files with paths
OBJS := $(OBJ_DIR)/Cache.o $(OBJ_DIR)/main.o

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile Cache.o from Cache.cpp
$(OBJ_DIR)/Cache.o: $(SRC_DIR)/Cache.cpp $(SRC_DIR)/Cache.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC_DIR)/Cache.cpp -o $(OBJ_DIR)/Cache.o

# Rule to compile main.o from main.cpp
$(OBJ_DIR)/main.o: main.cpp $(SRC_DIR)/Cache.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp -o $(OBJ_DIR)/main.o

# Create obj directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up build files
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

