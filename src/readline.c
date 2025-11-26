#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <conio.h>
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_BACKSPACE 8
#define KEY_ENTER 13
#define KEY_CTRL_C 3
#else
// Simple fallback for POSIX if compiled there (though we target Windows)
#include <unistd.h>
#endif

#include "readline.h"

#define HISTORY_MAX 100
#define BUFFER_SIZE 1024

static char *history[HISTORY_MAX];
static int history_count = 0;

void init_history(void) {
    history_count = 0;
    for (int i = 0; i < HISTORY_MAX; i++) {
        history[i] = NULL;
    }
}

void add_history(const char *line) {
    if (!line || strlen(line) == 0) return;
    
    // Don't add duplicates of the last command
    if (history_count > 0 && strcmp(history[history_count - 1], line) == 0) {
        return;
    }

    // If full, shift everything down (simple implementation)
    if (history_count >= HISTORY_MAX) {
        free(history[0]);
        for (int i = 1; i < HISTORY_MAX; i++) {
            history[i - 1] = history[i];
        }
        history_count--;
    }

    history[history_count++] = strdup(line);
}

void free_history(void) {
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }
    history_count = 0;
}

#ifdef _WIN32
// Windows implementation using _getch()

char *read_line(const char *prompt) {
    char buffer[BUFFER_SIZE];
    int pos = 0;
    int history_index = history_count; // Points to "new" line (after last history)
    int ch;
    
    // Display prompt
    printf("%s", prompt);
    
    // Initialize buffer
    buffer[0] = '\0';
    
    while (1) {
        ch = _getch();
        
        if (ch == 0 || ch == 0xE0) {
            // Special key
            ch = _getch(); // Get the actual code
            
            if (ch == KEY_UP) {
                // History Up
                if (history_index > 0) {
                    history_index--;
                    
                    // Clear current line
                    printf("\r%s", prompt);
                    for (int i = 0; i < pos; i++) printf(" "); // Wipe old chars
                    printf(" \r%s", prompt); // Return to start
                    
                    // Copy history to buffer
                    strncpy(buffer, history[history_index], BUFFER_SIZE - 1);
                    pos = strlen(buffer);
                    printf("%s", buffer);
                }
            } else if (ch == KEY_DOWN) {
                // History Down
                if (history_index < history_count) {
                    history_index++;
                    
                    // Clear current line
                    printf("\r%s", prompt);
                    for (int i = 0; i < pos; i++) printf(" ");
                    printf(" \r%s", prompt);
                    
                    if (history_index < history_count) {
                        strncpy(buffer, history[history_index], BUFFER_SIZE - 1);
                    } else {
                        buffer[0] = '\0'; // New empty line
                    }
                    pos = strlen(buffer);
                    printf("%s", buffer);
                }
            }
            // Ignore other special keys for now
            
        } else if (ch == KEY_BACKSPACE) {
            // Backspace
            if (pos > 0) {
                pos--;
                buffer[pos] = '\0';
                printf("\b \b"); // Erase character on screen
            }
        } else if (ch == KEY_ENTER) {
            // Enter
            buffer[pos] = '\0';
            printf("\n");
            return strdup(buffer);
        } else if (ch == KEY_CTRL_C) {
            // Ctrl+C
            printf("^C\n");
            buffer[0] = '\0';
            return strdup(buffer);
        } else if (isprint(ch)) {
            // Regular character
            if (pos < BUFFER_SIZE - 1) {
                buffer[pos++] = (char)ch;
                buffer[pos] = '\0';
                printf("%c", ch);
            }
        }
    }
}

#else
// POSIX fallback (simple fgets, no history navigation without raw mode)
char *read_line(const char *prompt) {
    char *line = NULL;
    size_t len = 0;
    
    printf("%s", prompt);
    if (getline(&line, &len, stdin) == -1) {
        free(line);
        return NULL;
    }
    
    // Remove newline
    size_t line_len = strlen(line);
    if (line_len > 0 && line[line_len - 1] == '\n') {
        line[line_len - 1] = '\0';
    }
    
    return line;
}
#endif
