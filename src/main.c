#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 1024

int main(void) {
    char input[MAX_INPUT];
    
    while (1) {
        // Display prompt
        printf("myshell> ");
        fflush(stdout);
        
        // Read input
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        // Exit command
        if (strcmp(input, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }
        
        // Echo the command for now
        if (strlen(input) > 0) {
            printf("You entered: %s\n", input);
        }
    }
    
    return 0;
}
