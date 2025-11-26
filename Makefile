CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
TARGET = myshell
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default target
all: $(TARGET)

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link object files to create executable
$(TARGET): $(OBJ_DIR) $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Rebuild
rebuild: clean all

.PHONY: all clean rebuild
