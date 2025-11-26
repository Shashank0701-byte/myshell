#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtins.h"

// List of built-in command names
char *builtin_names[] = {
    "cd",
    "exit"
};

// Number of built-ins
int num_builtins() {
    return sizeof(builtin_names) / sizeof(char *);
}

/**
 * Check if a command is a built-in
 */
int is_builtin(char *cmd) {
    if (cmd == NULL) {
        return 0;
    }
    
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(cmd, builtin_names[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Execute a built-in command
 */
int execute_builtin(char **argv) {
    if (argv == NULL || argv[0] == NULL) {
        return 1;
    }
    
    if (strcmp(argv[0], "cd") == 0) {
        return builtin_cd(argv);
    } else if (strcmp(argv[0], "exit") == 0) {
        return builtin_exit(argv);
    }
    
    return 1; // Unknown built-in
}

/**
 * Built-in: cd - Change directory
 */
int builtin_cd(char **argv) {
    if (argv[1] == NULL) {
        // No argument: go to HOME
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "myshell: cd: HOME not set\n");
            return 1;
        }
        if (chdir(home) != 0) {
            perror("myshell: cd");
            return 1;
        }
    } else {
        // Change to specified directory
        if (chdir(argv[1]) != 0) {
            perror("myshell: cd");
            return 1;
        }
    }
    return 0;
}

/**
 * Built-in: exit - Exit the shell
 */
int builtin_exit(char **argv) {
    // Optional: support exit code argument
    if (argv[1] != NULL) {
        int exit_code = atoi(argv[1]);
        printf("Goodbye! (exit code: %d)\n", exit_code);
        exit(exit_code);
    }
    
    printf("Goodbye!\n");
    return -1; // Signal to exit shell
}
