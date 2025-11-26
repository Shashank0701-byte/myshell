#!/bin/bash
# Build script for myshell

echo "Building myshell..."

# Create obj directory
mkdir -p obj

# Compile source files
gcc -Wall -Wextra -Iinclude -c src/main.c -o obj/main.o
gcc -Wall -Wextra -Iinclude -c src/builtins.c -o obj/builtins.o

# Link
gcc obj/main.o obj/builtins.o -o myshell

if [ $? -eq 0 ]; then
    echo "Build successful! Run with: ./myshell"
else
    echo "Build failed!"
    exit 1
fi
