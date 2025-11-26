#ifndef ERROR_H
#define ERROR_H

/**
 * Error handling utilities for myshell
 * Provides consistent error messages across the shell
 */

/**
 * Print error message for command not found
 * @param cmd: Command name that was not found
 */
void error_command_not_found(const char *cmd);

/**
 * Print error message for file not found (redirection)
 * @param filename: File that was not found
 * @param type: "input" or "output"
 */
void error_file_not_found(const char *filename, const char *type);

/**
 * Print error message for allocation failure
 * @param context: Context where allocation failed
 */
void error_allocation(const char *context);

/**
 * Print error message for fork failure
 */
void error_fork(void);

/**
 * Print error message for pipe failure
 */
void error_pipe(void);

/**
 * Print error message for exec failure
 * @param cmd: Command that failed to execute
 */
void error_exec(const char *cmd);

/**
 * Print error message for invalid syntax
 * @param message: Specific syntax error message
 */
void error_syntax(const char *message);

/**
 * Print error message for missing argument
 * @param cmd: Command that requires an argument
 */
void error_missing_arg(const char *cmd);

/**
 * Print error message for permission denied
 * @param resource: Resource that was denied
 */
void error_permission_denied(const char *resource);

/**
 * Print generic error with system error message
 * @param context: Context of the error
 */
void error_system(const char *context);

#endif // ERROR_H
