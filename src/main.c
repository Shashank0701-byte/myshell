#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            printf("Command: %s\n", line);
        }
    }
    
    // Free allocated memory
    free(line);
    
    return 0;
}
