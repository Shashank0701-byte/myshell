#ifndef READLINE_H
#define READLINE_H

/**
 * Initialize the history buffer
 */
void init_history(void);

/**
 * Add a line to the command history
 * @param line: The command line to add
 */
void add_history(const char *line);

/**
 * Read a line of input with support for history navigation (Up/Down arrows)
 * @param prompt: The prompt to display
 * @return: The read line (must be freed by caller) or NULL on EOF
 */
char *read_line(const char *prompt);

/**
 * Free history memory
 */
void free_history(void);

/**
 * Attempt to complete the current input
 * @param buffer: Current input buffer
 * @param pos: Current cursor position
 * @return: New cursor position after completion
 */
int complete_input(char *buffer, int pos);

#endif // READLINE_H
