#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    
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
        
        // Print back the command
        if (strlen(line) > 0) {
            // Tokenize the input
            char **tokens = tokenize(line);
            
            // Parse into command structure
            struct command *cmd = parse_command(tokens);
            
            if (cmd != NULL && cmd->argv[0] != NULL) {
                // Check if it's a built-in command
                if (is_builtin(cmd->argv[0])) {
                    int result = execute_builtin(cmd->argv);
                    if (result == -1) {
                        // Exit signal
                        free_command(cmd);
                        free(tokens);
                        break;
                    }
                } else {
                    // Display parsed command (for now, until we implement exec)
                    printf("Command: %s\n", line);
                    printf("  argv: ");
                    for (int i = 0; cmd->argv[i] != NULL; i++) {
                        printf("[%s] ", cmd->argv[i]);
                    }
                    printf("\n");
                    
                    if (cmd->input_file) {
                        printf("  input: %s\n", cmd->input_file);
                    }
                    if (cmd->output_file) {
                        printf("  output: %s\n", cmd->output_file);
                    }
                    if (cmd->background) {
                        printf("  background: yes\n");
                    }
                }
                
                // Free command structure
                free_command(cmd);
            }
            
            // Free tokens array
            free(tokens);
        }
    }
    
    // Free allocated memory
    free(line);
    
    return 0;
}
