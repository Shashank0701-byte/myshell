#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_DELIMITERS " \t\r\n\a"
#define TOKEN_BUFFER_SIZE 64

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
        
        // Exit command
        if (strcmp(line, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }
        
        // Print back the command
        if (strlen(line) > 0) {
            // Tokenize the input
            char **tokens = tokenize(line);
            
            // Print tokens
            printf("Command: %s\n", line);
            printf("Tokens: ");
            for (int i = 0; tokens[i] != NULL; i++) {
                printf("[%s] ", tokens[i]);
            }
            printf("\n");
            
            // Free tokens array (tokens themselves point to line, so don't free them)
            free(tokens);
        }
    }
    
    // Free allocated memory
    free(line);
    
    return 0;
}
