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
- **Command History**:
  - Use **Up/Down Arrows** to navigate previous commands.
  - History persists for the session.
- **Job Control** (POSIX only):
  - **`jobs`**: List all background and stopped jobs.
  - **`fg`**: Bring a job to the foreground.
  - **`bg`**: Continue a stopped job in the background.
  - Track and manage background processes.
- **Tab Completion**:
  - Press **Tab** to auto-complete commands, files, and directories.
  - Shows all matches if multiple options exist.
  - Completes to common prefix when possible.
- **Environment Variable Expansion**:
  - Supports `$VAR` and `${VAR}` syntax.
  - Variables are expanded in all command arguments.
  - Use `\$` to escape dollar signs.

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
gcc -Wall -Wextra -Iinclude -c src/readline.c -o obj/readline.o
gcc -Wall -Wextra -Iinclude -c src/jobs.c -o obj/jobs.o
gcc obj/main.o obj/builtins.o obj/error.o obj/readline.o obj/jobs.o -o myshell
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

**Environment Variables**
```bash
myshell> echo $HOME
myshell> echo ${USER}
myshell> cd $HOME/Documents
myshell> cat $HOME/.bashrc
```

**Tab Completion**
```bash
myshell> ec[TAB]      # Completes to "echo"
myshell> cd Doc[TAB]  # Completes to "Documents/" if it exists
myshell> cat RE[TAB]  # Shows: README.md  RESULTS.txt
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
[1] 12345
myshell> jobs
[1]+ Running		sleep 10
myshell> fg 1
sleep 10
# Press Ctrl+Z to stop
[1]+  Stopped		sleep 10
myshell> bg 1
[1]+ sleep 10 &
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
│   ├── error.c         # Centralized error handling
│   ├── readline.c      # Command history and input handling
│   └── jobs.c          # Job control system
├── include/
│   ├── builtins.h      # Headers for built-ins
│   ├── error.h         # Headers for error handling
│   ├── readline.h      # Headers for readline
│   └── jobs.h          # Headers for job control
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
6.  **Job Manager**: Tracks background jobs, handles `jobs`, `fg`, and `bg` commands.
7.  **Tab Completion**: Auto-completes commands and file paths using Windows FindFirstFile API.
8.  **Variable Expansion**: Expands `$VAR` and `${VAR}` environment variables in commands.

## ⚠️ Limitations

- **Scripting**: No support for control structures like `if` or `while`.
- **Windows Job Control**: `fg` and `bg` commands not supported on Windows.
- **Variable Assignment**: Cannot set/export custom variables (only expansion supported).

## 🤝 Contributing

Contributions are welcome! Feel free to submit pull requests or open issues for bugs and feature ideas.

## 📄 License

This project is open-source and available for educational and personal use.
