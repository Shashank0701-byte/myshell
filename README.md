# myshell - A Simple Unix Shell

A custom shell implementation in C with support for command execution, I/O redirection, piping, and background processes.

## Features

### ✅ Core Functionality
- **REPL Loop** - Read-Eval-Print Loop with `myshell>` prompt
- **Command Tokenization** - Dynamic parsing with `getline()`
- **Built-in Commands** - `cd`, `exit`
- **External Commands** - Execute any system command

### ✅ I/O Redirection
- **Input Redirection** (`<`) - Read input from file
- **Output Redirection** (`>`) - Write output to file
- **Combined** - Use both together

### ✅ Piping
- **Command Chaining** (`|`) - Connect multiple commands
- **Full POSIX Support** - Uses `pipe()`, `fork()`, and `dup2()`
- **Windows Note** - Limited pipe support on Windows (executes sequentially)

### ✅ Background Execution
- **Background Processes** (`&`) - Run commands in background
- **PID Display** - Shows process ID for background jobs

### ✅ Signal Handling
- **Ctrl+C Protection** - Shell ignores SIGINT, stays alive
- **Child Interruption** - Foreground commands can be interrupted
- **Clean Prompt** - Displays new prompt after Ctrl+C

### ✅ Error Handling
- **Consistent Messages** - All errors follow same format
- **Informative** - Includes context and errno details
- **User-Friendly** - Clear, helpful error descriptions

## Building

### Using build script:
```bash
chmod +x build.sh
./build.sh
```

### Manual build:
```bash
mkdir -p obj
gcc -Wall -Wextra -Iinclude -c src/main.c -o obj/main.o
gcc -Wall -Wextra -Iinclude -c src/builtins.c -o obj/builtins.o
gcc obj/main.o obj/builtins.o -o myshell
```

## Usage Examples

### Basic Commands
```bash
myshell> ls -la
myshell> pwd
myshell> echo "Hello World"
```

### Built-in Commands
```bash
myshell> cd /home/user
myshell> cd ..
myshell> exit
```

### I/O Redirection
```bash
# Output redirection
myshell> ls -la > files.txt
myshell> echo "test" > output.txt

# Input redirection
myshell> wc -l < input.txt
myshell> sort < unsorted.txt

# Both together
myshell> sort < input.txt > sorted.txt
```

### Piping
```bash
# Simple pipe (2 commands)
myshell> ls -la | grep txt

# Multiple pipes (3 commands)
myshell> cat file.txt | grep error | wc -l

# Even more pipes (4+ commands)
myshell> ps aux | grep user | awk '{print $2}' | sort | head -5

# With redirection
myshell> cat < input.txt | sort | uniq > output.txt

# Complex pipeline
myshell> cat /var/log/syslog | grep ERROR | cut -d' ' -f5- | sort | uniq -c | sort -rn
```

**Note**: Supports **unlimited number of pipes** using loop-based implementation!

### Background Execution
```bash
myshell> sleep 10 &
[Background] PID: 1234

myshell> long_running_command &
```

## Architecture

### File Structure
```
myshell/
├── src/
│   ├── main.c          # Main shell logic, REPL, parsing, execution
│   └── builtins.c      # Built-in command implementations
├── include/
│   └── builtins.h      # Built-in command headers
├── obj/                # Object files (generated)
├── Makefile            # Build configuration
├── build.sh            # Build script
└── .gitignore          # Git ignore rules
```

### Key Components

#### 1. **Tokenizer** (`tokenize()`)
- Splits input by whitespace using `strtok()`
- Dynamic memory allocation
- Returns NULL-terminated array

#### 2. **Parser** (`parse_command()`)
- Parses tokens into `struct command`
- Handles `<`, `>`, `&` operators
- Separates arguments from operators

#### 3. **Pipeline Splitter** (`split_pipeline()`)
- Splits tokens by `|` symbol
- Creates array of command structures
- Supports multiple pipes

#### 4. **Executor** (`execute_pipeline()`)
- **POSIX**: Uses `fork()`, `pipe()`, `dup2()`, `execvp()`
- **Windows**: Limited support with `_spawnvp()`
- Handles I/O redirection
- Waits for completion or backgrounds

#### 5. **Built-ins** (`execute_builtin()`)
- Executes without forking
- Direct shell process modification
- Currently: `cd`, `exit`

## Platform Support

### POSIX (Linux, macOS, Unix)
- ✅ Full pipe support with `fork()`/`exec()`
- ✅ All features fully functional

### Windows (MinGW, Git Bash)
- ✅ Basic commands
- ✅ I/O redirection
- ⚠️ Limited pipe support (sequential execution fallback)
- ✅ Background execution

## Technical Details

### Process Management
- **POSIX**: `fork()` + `execvp()` + `waitpid()`
- **Windows**: `_spawnvp()` with `_P_WAIT` / `_P_NOWAIT`

### File Descriptors
- **POSIX**: `open()`, `dup2()`, `close()`
- **Windows**: `_open()`, `_dup2()`, `_close()`

### Pipe Implementation
```c
// Create pipes
pipe(pipefds + i * 2);

// Child: redirect stdin from previous pipe
dup2(pipefds[(i-1) * 2], STDIN_FILENO);

// Child: redirect stdout to next pipe
dup2(pipefds[i * 2 + 1], STDOUT_FILENO);

// Execute command
execvp(argv[0], argv);
```

## Limitations

1. **No job control** - Can't bring background jobs to foreground (fg, bg, jobs)
2. **No job suspension** - Ctrl+Z doesn't suspend processes
3. **No command history** - No up/down arrow support
4. **No tab completion** - No auto-complete
5. **Windows pipes** - Limited to sequential execution
6. **No environment variables** - Can't set/export variables
7. **No scripting** - No if/while/for loops

## Future Enhancements

- [ ] Signal handling (SIGINT, SIGTSTP)
- [ ] Job control (fg, bg, jobs)
- [ ] Command history with readline
- [ ] Tab completion
- [ ] Environment variable support
- [ ] Scripting support (if, while, for)
- [ ] Append redirection (`>>`)
- [ ] Error redirection (`2>`, `2>&1`)
- [ ] Command substitution
- [ ] Wildcards and globbing

## License

Educational project - free to use and modify.

## Author

Created as a learning project for understanding Unix shell internals.
