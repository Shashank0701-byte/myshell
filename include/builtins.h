#ifndef BUILTINS_H
#define BUILTINS_H

/**
 * Check if a command is a built-in
 * @param cmd: Command name to check
 * @return: 1 if built-in, 0 otherwise
 */
int is_builtin(char *cmd);

/**
 * Execute a built-in command
 * @param argv: Command arguments (NULL-terminated)
 * @return: 0 on success, 1 on failure, -1 to exit shell
 */
int execute_builtin(char **argv);

/**
 * Built-in: cd - Change directory
 * @param argv: Command arguments
 * @return: 0 on success, 1 on failure
 */
int builtin_cd(char **argv);

/**
 * Built-in: exit - Exit the shell
 * @param argv: Command arguments
 * @return: -1 to signal shell exit
 */
int builtin_exit(char **argv);

/**
 * Built-in: jobs - List all jobs
 * @param argv: Command arguments
 * @return: 0 on success
 */
int builtin_jobs(char **argv);

/**
 * Built-in: fg - Bring job to foreground
 * @param argv: Command arguments
 * @return: 0 on success, 1 on failure
 */
int builtin_fg(char **argv);

/**
 * Built-in: bg - Continue job in background
 * @param argv: Command arguments
 * @return: 0 on success, 1 on failure
 */
int builtin_bg(char **argv);

#endif // BUILTINS_H
