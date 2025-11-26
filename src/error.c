#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "error.h"

#define SHELL_NAME "myshell"

/**
 * Print error message for command not found
 */
void error_command_not_found(const char *cmd) {
    fprintf(stderr, "%s: %s: command not found\n", SHELL_NAME, cmd);
}

/**
 * Print error message for file not found (redirection)
 */
void error_file_not_found(const char *filename, const char *type) {
    fprintf(stderr, "%s: %s redirection: %s: No such file or directory\n", 
            SHELL_NAME, type, filename);
}

/**
 * Print error message for allocation failure
 */
void error_allocation(const char *context) {
    fprintf(stderr, "%s: memory allocation error", SHELL_NAME);
    if (context) {
        fprintf(stderr, " in %s", context);
    }
    fprintf(stderr, "\n");
}

/**
 * Print error message for fork failure
 */
void error_fork(void) {
    fprintf(stderr, "%s: fork: failed to create process", SHELL_NAME);
    if (errno) {
        fprintf(stderr, ": %s", strerror(errno));
    }
    fprintf(stderr, "\n");
}

/**
 * Print error message for pipe failure
 */
void error_pipe(void) {
    fprintf(stderr, "%s: pipe: failed to create pipe", SHELL_NAME);
    if (errno) {
        fprintf(stderr, ": %s", strerror(errno));
    }
    fprintf(stderr, "\n");
}

/**
 * Print error message for exec failure
 */
void error_exec(const char *cmd) {
    fprintf(stderr, "%s: exec: failed to execute '%s'", SHELL_NAME, cmd);
    if (errno) {
        fprintf(stderr, ": %s", strerror(errno));
    }
    fprintf(stderr, "\n");
}

/**
 * Print error message for invalid syntax
 */
void error_syntax(const char *message) {
    fprintf(stderr, "%s: syntax error: %s\n", SHELL_NAME, message);
}

/**
 * Print error message for missing argument
 */
void error_missing_arg(const char *cmd) {
    fprintf(stderr, "%s: %s: missing argument\n", SHELL_NAME, cmd);
}

/**
 * Print error message for permission denied
 */
void error_permission_denied(const char *resource) {
    fprintf(stderr, "%s: %s: Permission denied\n", SHELL_NAME, resource);
}

/**
 * Print generic error with system error message
 */
void error_system(const char *context) {
    fprintf(stderr, "%s: %s", SHELL_NAME, context);
    if (errno) {
        fprintf(stderr, ": %s", strerror(errno));
    }
    fprintf(stderr, "\n");
}
