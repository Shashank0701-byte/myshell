#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "builtins.h"
#include "error.h"
#include "jobs.h"


// List of built-in command names
char *builtin_names[] = {
    "cd",
    "exit",
    "jobs",
    "fg",
    "bg"
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
    } else if (strcmp(argv[0], "jobs") == 0) {
        return builtin_jobs(argv);
    } else if (strcmp(argv[0], "fg") == 0) {
        return builtin_fg(argv);
    } else if (strcmp(argv[0], "bg") == 0) {
        return builtin_bg(argv);
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
            // Try USERPROFILE on Windows
            home = getenv("USERPROFILE");
        }
        if (home == NULL) {
            error_syntax("cd: HOME environment variable not set");
            return 1;
        }
        if (_chdir(home) != 0) {
            error_system("cd");
            return 1;
        }
    } else {
        // Change to specified directory
        if (_chdir(argv[1]) != 0) {
            error_system("cd");
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

/**
 * Built-in: jobs - List all jobs
 */
int builtin_jobs(char **argv) {
    (void)argv; // Unused parameter
    list_jobs();
    return 0;
}

/**
 * Built-in: fg - Bring job to foreground
 */
int builtin_fg(char **argv) {
    int job_id = 0;
    
    // Parse job ID if provided
    if (argv[1] != NULL) {
        if (argv[1][0] == '%') {
            job_id = atoi(argv[1] + 1);
        } else {
            job_id = atoi(argv[1]);
        }
    }
    
    return fg_job(job_id);
}

/**
 * Built-in: bg - Continue job in background
 */
int builtin_bg(char **argv) {
    int job_id = 0;
    
    // Parse job ID if provided
    if (argv[1] != NULL) {
        if (argv[1][0] == '%') {
            job_id = atoi(argv[1] + 1);
        } else {
            job_id = atoi(argv[1]);
        }
    }
    
    return bg_job(job_id);
}
