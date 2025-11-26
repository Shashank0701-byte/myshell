# Error Handling in myshell

## Overview

The shell now has a centralized error handling system that provides consistent error messages across all components.

## Error Module

### Files
- **`include/error.h`** - Error function declarations
- **`src/error.c`** - Error function implementations

### Functions

#### `error_command_not_found(const char *cmd)`
**Usage:** When a command cannot be executed
```c
error_command_not_found("invalidcmd");
// Output: myshell: invalidcmd: command not found
```

#### `error_file_not_found(const char *filename, const char *type)`
**Usage:** When a file for redirection doesn't exist
```c
error_file_not_found("input.txt", "input");
// Output: myshell: input redirection: input.txt: No such file or directory
```

#### `error_allocation(const char *context)`
**Usage:** When memory allocation fails
```c
error_allocation("parse_command");
// Output: myshell: memory allocation error in parse_command
```

#### `error_fork(void)`
**Usage:** When fork() fails
```c
error_fork();
// Output: myshell: fork: failed to create process: <errno message>
```

#### `error_pipe(void)`
**Usage:** When pipe() fails
```c
error_pipe();
// Output: myshell: pipe: failed to create pipe: <errno message>
```

#### `error_exec(const char *cmd)`
**Usage:** When execvp() fails
```c
error_exec("ls");
// Output: myshell: exec: failed to execute 'ls': <errno message>
```

#### `error_syntax(const char *message)`
**Usage:** For syntax errors
```c
error_syntax("unexpected token '|'");
// Output: myshell: syntax error: unexpected token '|'
```

#### `error_missing_arg(const char *cmd)`
**Usage:** When a command requires an argument
```c
error_missing_arg("cd");
// Output: myshell: cd: missing argument
```

#### `error_permission_denied(const char *resource)`
**Usage:** For permission errors
```c
error_permission_denied("/etc/shadow");
// Output: myshell: /etc/shadow: Permission denied
```

#### `error_system(const char *context)`
**Usage:** Generic system error with errno
```c
error_system("cd");
// Output: myshell: cd: <errno message>
```

## Error Message Format

All error messages follow a consistent format:

```
myshell: <context>: <specific error>
```

### Examples

| Error Type | Example Output |
|------------|----------------|
| Command not found | `myshell: foo: command not found` |
| File not found | `myshell: input redirection: file.txt: No such file or directory` |
| Allocation error | `myshell: memory allocation error in tokenize` |
| Fork failure | `myshell: fork: failed to create process: Resource temporarily unavailable` |
| Pipe failure | `myshell: pipe: failed to create pipe: Too many open files` |
| Syntax error | `myshell: syntax error: unexpected end of input` |
| Missing argument | `myshell: cd: missing argument` |
| Permission denied | `myshell: /root: Permission denied` |

## Benefits

### 1. Consistency
All error messages have the same format and prefix (`myshell:`).

### 2. Maintainability
Error messages are centralized in one module, easy to update.

### 3. Errno Integration
System errors automatically include errno messages when available.

### 4. Context Information
Each error includes context about where it occurred.

### 5. User-Friendly
Clear, descriptive messages help users understand what went wrong.

## Usage in Code

### Before (Inconsistent)
```c
fprintf(stderr, "myshell: allocation error\n");
perror("myshell: fork");
fprintf(stderr, "myshell: %s: command not found\n", cmd);
```

### After (Consistent)
```c
error_allocation("tokenize");
error_fork();
error_command_not_found(cmd);
```

## Error Categories

### 1. System Errors
- Fork failures
- Pipe failures
- File I/O errors
- Permission errors

### 2. User Errors
- Command not found
- Missing arguments
- Syntax errors
- Invalid paths

### 3. Internal Errors
- Memory allocation failures
- Unexpected states

## Testing

### Test Command Not Found
```bash
myshell> invalidcommand
myshell: invalidcommand: command not found
```

### Test Missing File
```bash
myshell> cat < nonexistent.txt
myshell: input redirection: nonexistent.txt: No such file or directory
```

### Test Permission Denied
```bash
myshell> cd /root
myshell: cd: Permission denied
```

### Test Syntax Error
```bash
myshell> | grep test
myshell: syntax error: unexpected token '|'
```

## Implementation Details

### Error Prefix
All errors use the constant `SHELL_NAME`:
```c
#define SHELL_NAME "myshell"
```

### Errno Integration
System errors automatically append errno messages:
```c
if (errno) {
    fprintf(stderr, ": %s", strerror(errno));
}
```

### Flexibility
Functions accept context parameters for detailed error reporting:
```c
error_allocation("parse_command argv");
// vs
error_allocation(NULL);  // Generic message
```

## Files Modified

| File | Changes |
|------|---------|
| `src/main.c` | Replaced all error messages with error functions |
| `src/builtins.c` | Updated cd error handling |
| `include/error.h` | New error function declarations |
| `src/error.c` | New error function implementations |
| `build.sh` | Added error.c compilation |

## Summary

✅ **Centralized** - All error handling in one module  
✅ **Consistent** - Same format for all errors  
✅ **Informative** - Includes context and errno messages  
✅ **Maintainable** - Easy to update and extend  
✅ **User-Friendly** - Clear, helpful error messages  

The shell now provides professional-quality error messages! 🎉
