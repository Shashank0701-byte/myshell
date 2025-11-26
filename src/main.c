#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
// Windows headers
#include <process.h>
#include <io.h>
#include <fcntl.h>
#else
// POSIX headers
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

#include "builtins.h"






#define TOKEN_DELIMITERS " \t\r\n\a"
#define TOKEN_BUFFER_SIZE 64

/**
 * Structure to represent a parsed command
 */
struct command {
    char **argv;           // Command arguments (NULL-terminated)
    char *input_file;      // Input redirection file (or NULL)
    char *output_file;     // Output redirection file (or NULL)
    int background;        // 1 if background (&), 0 otherwise
};

// Forward declarations
int execute_external(struct command *cmd);

/**
 * Signal handler for SIGINT (Ctrl+C)
 * Ignores the signal in the shell, but children will have default behavior
 */
void sigint_handler(int sig) {
    (void)sig;  // Unused parameter
    // Print newline for clean prompt
    printf("\n");
    // Display prompt again
    printf("myshell> ");
    fflush(stdout);
}

/**
 * Setup signal handlers for the shell
 */
void setup_signal_handlers(void) {
    // Ignore SIGINT (Ctrl+C) in the shell
    signal(SIGINT, sigint_handler);
    
    // Optionally ignore SIGQUIT (Ctrl+\) as well
    #ifndef _WIN32
    signal(SIGQUIT, SIG_IGN);
    #endif
}

/**
 * Initialize a command structure
 */
struct command *init_command(void) {
    struct command *cmd = malloc(sizeof(struct command));
    if (!cmd) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    cmd->argv = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->background = 0;
    return cmd;
}

/**
 * Free a command structure
 */
void free_command(struct command *cmd) {
    if (cmd) {
        free(cmd->argv);
        free(cmd);
    }
}

/**
 * Parse tokens into a command structure
 * Handles: redirection (<, >), background (&)
 */
struct command *parse_command(char **tokens) {
    if (tokens == NULL || tokens[0] == NULL) {
        return NULL;
    }
    
    struct command *cmd = init_command();
    int argc = 0;
    int argv_size = TOKEN_BUFFER_SIZE;
    
    // Allocate argv array
    cmd->argv = malloc(argv_size * sizeof(char*));
    if (!cmd->argv) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    // Parse tokens
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "<") == 0) {
            // Input redirection
            if (tokens[i + 1] != NULL) {
                cmd->input_file = tokens[i + 1];
                i++; // Skip the filename
            }
        } else if (strcmp(tokens[i], ">") == 0) {
            // Output redirection
            if (tokens[i + 1] != NULL) {
                cmd->output_file = tokens[i + 1];
                i++; // Skip the filename
            }
        } else if (strcmp(tokens[i], "&") == 0) {
            // Background execution
            cmd->background = 1;
        } else {
            // Regular argument
            cmd->argv[argc] = tokens[i];
            argc++;
            
            // Reallocate if needed
            if (argc >= argv_size) {
                argv_size += TOKEN_BUFFER_SIZE;
                cmd->argv = realloc(cmd->argv, argv_size * sizeof(char*));
                if (!cmd->argv) {
                    fprintf(stderr, "myshell: allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    
    // NULL-terminate argv
    cmd->argv[argc] = NULL;
    
    return cmd;
}


/**
 * Tokenize input string by whitespace
 * @param line: Input string to tokenize
 * @return: NULL-terminated array of tokens (char**)
 */
char **tokenize(char *line) {
    int bufsize = TOKEN_BUFFER_SIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    
    if (!tokens) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    // Use strtok to split by whitespace
    token = strtok(line, TOKEN_DELIMITERS);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        
        // Reallocate if we exceed buffer
        if (position >= bufsize) {
            bufsize += TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        
        token = strtok(NULL, TOKEN_DELIMITERS);
    }
    
    // NULL-terminate the array
    tokens[position] = NULL;
    return tokens;
}

/**
 * Split tokens into pipeline commands (separated by |)
 * @param tokens: Array of tokens
 * @param commands: Output array of command structures
 * @return: Number of commands in pipeline
 */
int split_pipeline(char **tokens, struct command ***commands) {
    int num_cmds = 0;
    int cmd_capacity = 4;
    int token_start = 0;
    int i;
    
    *commands = malloc(cmd_capacity * sizeof(struct command*));
    if (!*commands) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    // Find pipe symbols and create commands
    for (i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            // Create a temporary array for this command's tokens
            int token_count = i - token_start;
            char **cmd_tokens = malloc((token_count + 1) * sizeof(char*));
            for (int j = 0; j < token_count; j++) {
                cmd_tokens[j] = tokens[token_start + j];
            }
            cmd_tokens[token_count] = NULL;
            
            // Parse this command
            (*commands)[num_cmds] = parse_command(cmd_tokens);
            free(cmd_tokens);
            num_cmds++;
            
            // Expand array if needed
            if (num_cmds >= cmd_capacity) {
                cmd_capacity *= 2;
                *commands = realloc(*commands, cmd_capacity * sizeof(struct command*));
                if (!*commands) {
                    fprintf(stderr, "myshell: allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }
            
            token_start = i + 1;
        }
    }
    
    // Handle the last command (or only command if no pipes)
    if (token_start <= i) {
        int token_count = i - token_start;
        char **cmd_tokens = malloc((token_count + 1) * sizeof(char*));
        for (int j = 0; j < token_count; j++) {
            cmd_tokens[j] = tokens[token_start + j];
        }
        cmd_tokens[token_count] = NULL;
        
        (*commands)[num_cmds] = parse_command(cmd_tokens);
        free(cmd_tokens);
        num_cmds++;
    }
    
    return num_cmds;
}

/**
 * Execute a pipeline of commands
 * @param commands: Array of command structures
 * @param num_cmds: Number of commands in pipeline
 * @return: 0 on success, 1 on failure
 * 
 * Note: Full pipeline support requires fork/exec which is not available on Windows.
 * On Windows, this provides limited functionality.
 */
int execute_pipeline(struct command **commands, int num_cmds) {
    if (num_cmds == 0) {
        return 1;
    }
    
    // Single command - no pipes needed
    if (num_cmds == 1) {
        if (commands[0]->argv[0] && is_builtin(commands[0]->argv[0])) {
            return execute_builtin(commands[0]->argv);
        }
        return execute_external(commands[0]);
    }
    
    // Multiple commands with pipes
    #ifdef _WIN32
    // Windows: Limited pipe support
    fprintf(stderr, "myshell: piping not fully supported on Windows\n");
    fprintf(stderr, "myshell: executing commands sequentially instead\n");
    
    // Execute commands sequentially as a fallback
    for (int i = 0; i < num_cmds; i++) {
        if (commands[i]->argv[0]) {
            if (is_builtin(commands[i]->argv[0])) {
                execute_builtin(commands[i]->argv);
            } else {
                execute_external(commands[i]);
            }
        }
    }
    return 0;
    
    #else
    // POSIX: Full pipe support with fork/exec
    int i;
    int pipefds[2 * (num_cmds - 1)];
    pid_t pid;
    int status;
    
    // Create all pipes
    for (i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("myshell: pipe");
            return 1;
        }
    }
    
    // Execute each command in the pipeline
    for (i = 0; i < num_cmds; i++) {
        pid = fork();
        
        if (pid < 0) {
            perror("myshell: fork");
            return 1;
        } else if (pid == 0) {
            // Child process
            
            // Restore default signal handlers in child
            signal(SIGINT, SIG_DFL);
            #ifndef _WIN32
            signal(SIGQUIT, SIG_DFL);
            #endif
            
            // If not the first command, get input from previous pipe
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("myshell: dup2");
                    exit(EXIT_FAILURE);
                }
            }
            
            // If not the last command, send output to next pipe
            if (i < num_cmds - 1) {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("myshell: dup2");
                    exit(EXIT_FAILURE);
                }
            }
            
            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                close(pipefds[j]);
            }
            
            // Handle I/O redirection for first/last commands
            if (i == 0 && commands[i]->input_file) {
                int fd_in = open(commands[i]->input_file, O_RDONLY);
                if (fd_in < 0) {
                    perror("myshell: input redirection");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            
            if (i == num_cmds - 1 && commands[i]->output_file) {
                int fd_out = open(commands[i]->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    perror("myshell: output redirection");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            
            // Execute the command
            if (execvp(commands[i]->argv[0], commands[i]->argv) < 0) {
                fprintf(stderr, "myshell: %s: command not found\n", commands[i]->argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }
    
    // Parent: Close all pipe file descriptors
    for (i = 0; i < 2 * (num_cmds - 1); i++) {
        close(pipefds[i]);
    }
    
    // Wait for all children
    for (i = 0; i < num_cmds; i++) {
        wait(&status);
    }
    
    return 0;
    #endif
}

/**
 * Execute an external command using _spawnvp (Windows-compatible)
 * @param cmd: Command structure with argv, redirection, and background info
 * @return: 0 on success, 1 on failure
 */
int execute_external(struct command *cmd) {
    int status;
    int saved_stdin = -1, saved_stdout = -1;
    int fd_in = -1, fd_out = -1;
    
    if (cmd == NULL || cmd->argv == NULL || cmd->argv[0] == NULL) {
        return 1;
    }
    
    // Handle input redirection
    if (cmd->input_file != NULL) {
        saved_stdin = _dup(0);  // Save stdin
        fd_in = _open(cmd->input_file, _O_RDONLY);
        if (fd_in < 0) {
            perror("myshell: input redirection");
            return 1;
        }
        _dup2(fd_in, 0);  // Redirect stdin
        _close(fd_in);
    }
    
    // Handle output redirection
    if (cmd->output_file != NULL) {
        saved_stdout = _dup(1);  // Save stdout
        fd_out = _open(cmd->output_file, _O_WRONLY | _O_CREAT | _O_TRUNC, 0644);
        if (fd_out < 0) {
            perror("myshell: output redirection");
            if (saved_stdin >= 0) {
                _dup2(saved_stdin, 0);
                _close(saved_stdin);
            }
            return 1;
        }
        _dup2(fd_out, 1);  // Redirect stdout
        _close(fd_out);
    }
    
    // Spawn the process
    if (cmd->background) {
        // Background process - don't wait
        status = _spawnvp(_P_NOWAIT, cmd->argv[0], (const char* const*)cmd->argv);
        if (status == -1) {
            fprintf(stderr, "myshell: %s: command not found\n", cmd->argv[0]);
        } else {
            printf("[Background] PID: %d\n", status);
        }
    } else {
        // Foreground process - wait for completion
        status = _spawnvp(_P_WAIT, cmd->argv[0], (const char* const*)cmd->argv);
        if (status == -1) {
            fprintf(stderr, "myshell: %s: command not found\n", cmd->argv[0]);
        }
    }
    
    // Restore stdin/stdout
    if (saved_stdin >= 0) {
        _dup2(saved_stdin, 0);
        _close(saved_stdin);
    }
    if (saved_stdout >= 0) {
        _dup2(saved_stdout, 1);
        _close(saved_stdout);
    }
    
    return (status == -1) ? 1 : 0;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    
    // Setup signal handlers
    setup_signal_handlers();
    
    // REPL: Read-Eval-Print Loop
    while (1) {
        // Display prompt
        printf("myshell> ");
        fflush(stdout);
        
        // Read user input using getline
        nread = getline(&line, &len, stdin);
        
        // Handle EOF (Ctrl+D) or error
        if (nread == -1) {
            printf("\n");
            break;
        }
        
        // Remove trailing newline
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }
        
        // Process the command
        if (strlen(line) > 0) {
            // Tokenize the input
            char **tokens = tokenize(line);
            
            // Split into pipeline commands
            struct command **commands = NULL;
            int num_cmds = split_pipeline(tokens, &commands);
            
            // Check for exit in first command
            if (num_cmds > 0 && commands[0]->argv[0] != NULL) {
                if (strcmp(commands[0]->argv[0], "exit") == 0) {
                    int result = execute_builtin(commands[0]->argv);
                    if (result == -1) {
                        // Cleanup and exit
                        for (int i = 0; i < num_cmds; i++) {
                            free_command(commands[i]);
                        }
                        free(commands);
                        free(tokens);
                        break;
                    }
                }
            }
            
            // Execute the pipeline
            if (num_cmds > 0) {
                execute_pipeline(commands, num_cmds);
                
                // Free all commands
                for (int i = 0; i < num_cmds; i++) {
                    free_command(commands[i]);
                }
                free(commands);
            }
            
            // Free tokens array
            free(tokens);
        }
    }
    
    // Free allocated memory
    free(line);
    
    return 0;
}
