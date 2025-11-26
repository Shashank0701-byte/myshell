#!/bin/bash
# Build script for myshell

echo "Building myshell..."

# Create obj directory
mkdir -p obj

# Compile source files
echo "Compiling main.c..."
gcc -Wall -Wextra -Iinclude -c src/main.c -o obj/main.o || exit 1

echo "Compiling builtins.c..."
gcc -Wall -Wextra -Iinclude -c src/builtins.c -o obj/builtins.o || exit 1

echo "Compiling error.c..."
gcc -Wall -Wextra -Iinclude -c src/error.c -o obj/error.o || exit 1

echo "Compiling readline.c..."
gcc -Wall -Wextra -Iinclude -c src/readline.c -o obj/readline.o || exit 1

echo "Compiling jobs.c..."
gcc -Wall -Wextra -Iinclude -c src/jobs.c -o obj/jobs.o || exit 1

# Link
echo "Linking..."
gcc obj/main.o obj/builtins.o obj/error.o obj/readline.o obj/jobs.o -o myshell || exit 1

echo "✓ Build successful! Run with: ./myshell"

