# myshell

A robust, custom Unix shell implementation in C, designed for educational purposes and practical use. It features a REPL loop, command execution, I/O redirection, piping, background processes, and more.

## 🚀 Features

### Core Functionality
- **REPL Interface**: Interactive Read-Eval-Print Loop with a custom prompt.
- **Command Execution**: Seamlessly executes external system commands.
- **Built-in Commands**: Native support for `cd` (change directory) and `exit`.
- **Cross-Platform**: Runs on POSIX systems (Linux, macOS) and Windows (MinGW).

### Advanced Features
- **I/O Redirection**:
  - Input (`<`): Read from files.
  - Output (`>`): Write to files.
  - Combined: `sort < input.txt > output.txt`.
- **Piping**:
  - Chain multiple commands: `cmd1 | cmd2 | cmd3`.
  - Supports unlimited pipe depth.
- **Background Execution**:
  - Run commands asynchronously using `&`.
  - Displays PID for background jobs.
- **Signal Handling**:
  - **Ctrl+C Protection**: The shell ignores SIGINT, preventing accidental termination.
  - **Graceful Interruption**: Foreground commands can be interrupted without killing the shell.
- **Configuration**:
  - Loads startup commands from `~/.myshellrc`.
  - Customize your environment automatically on launch.
- **Robust Error Handling**:
  - Consistent, informative error messages.
  - Detailed system error reporting (errno).

## 🛠️ Installation & Build

### Prerequisites
- GCC Compiler
- Make (optional)

### Quick Build
```bash
./build.sh
```

### Manual Build
```bash
mkdir -p obj
gcc -Wall -Wextra -Iinclude -c src/main.c -o obj/main.o
gcc -Wall -Wextra -Iinclude -c src/builtins.c -o obj/builtins.o
gcc -Wall -Wextra -Iinclude -c src/error.c -o obj/error.o
gcc obj/main.o obj/builtins.o obj/error.o -o myshell
```

## 📖 Usage

Start the shell:
```bash
./myshell
```

### Examples

**File Operations**
```bash
myshell> ls -la
myshell> pwd
myshell> cat README.md
```

**Redirection & Piping**
```bash
# Redirect output
myshell> echo "Hello" > file.txt

# Pipe commands
myshell> cat file.txt | grep "H" | wc -l

# Complex pipeline
myshell> ps aux | grep user | sort | head -5
```

**Background Jobs**
```bash
myshell> sleep 10 &
[Background] PID: 12345
```

**Configuration**
Create a `~/.myshellrc` file to run commands at startup:
```bash
# ~/.myshellrc
echo "Welcome to myshell!"
date
```

## 🏗️ Architecture

The project is structured for modularity and maintainability:

```
myshell/
├── src/
│   ├── main.c          # Core logic: REPL, parser, executor
│   ├── builtins.c      # Built-in command implementations
│   └── error.c         # Centralized error handling
├── include/
│   ├── builtins.h      # Headers for built-ins
│   └── error.h         # Headers for error handling
├── obj/                # Compiled object files
├── build.sh            # Build automation script
└── README.md           # Documentation
```

### Key Components

1.  **Tokenizer**: Splits input strings into tokens using `strtok`.
2.  **Parser**: Converts tokens into `struct command` objects, handling redirection and background flags.
3.  **Pipeline Splitter**: Breaks command chains by the pipe symbol `|`.
4.  **Executor**:
    *   **POSIX**: Uses `fork()`, `pipe()`, `dup2()`, and `execvp()` for full functionality.
    *   **Windows**: Uses `_spawnvp()` with platform-specific adaptations.
5.  **Signal Handler**: Manages `SIGINT` to protect the shell process.

## ⚠️ Limitations

- **Job Control**: No support for `fg`, `bg`, or `jobs` commands yet.
- **Command History**: No up/down arrow navigation (requires readline integration).
- **Tab Completion**: Not currently implemented.
- **Environment Variables**: Cannot set/export custom variables (except via `cd`).
- **Scripting**: No support for control structures like `if` or `while`.

## 🤝 Contributing

Contributions are welcome! Feel free to submit pull requests or open issues for bugs and feature ideas.

## 📄 License

This project is open-source and available for educational and personal use.
