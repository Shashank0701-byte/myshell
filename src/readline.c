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

// Tab completion support
#ifdef _WIN32
#include <windows.h>
#include <direct.h>

/**
 * Get list of files/directories matching prefix
 */
static int get_file_matches(const char *prefix, char matches[][256], int max_matches) {
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    char search_path[512];
    int count = 0;
    
    // Build search pattern
    snprintf(search_path, sizeof(search_path), "%s*", prefix);
    
    hFind = FindFirstFile(search_path, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    do {
        // Skip . and ..
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
            continue;
        }
        
        // Add to matches
        if (count < max_matches) {
            strncpy(matches[count], findData.cFileName, 255);
            matches[count][255] = '\0';
            count++;
        }
    } while (FindNextFile(hFind, &findData) && count < max_matches);
    
    FindClose(hFind);
    return count;
}

/**
 * Get list of commands matching prefix (built-ins and PATH executables)
 */
static int get_command_matches(const char *prefix, char matches[][256], int max_matches) {
    int count = 0;
    const char *builtins[] = {"cd", "exit", "jobs", "fg", "bg", NULL};
    
    // Check built-in commands
    for (int i = 0; builtins[i] != NULL && count < max_matches; i++) {
        if (strncmp(builtins[i], prefix, strlen(prefix)) == 0) {
            strncpy(matches[count], builtins[i], 255);
            matches[count][255] = '\0';
            count++;
        }
    }
    
    // TODO: Could also search PATH for executables
    // For now, just return built-ins
    
    return count;
}

/**
 * Complete the current input
 */
int complete_input(char *buffer, int pos) {
    char matches[50][256];
    int match_count = 0;
    char prefix[256] = "";
    int prefix_start = 0;
    
    // Find the word to complete (from last space to cursor)
    for (int i = pos - 1; i >= 0; i--) {
        if (buffer[i] == ' ' || buffer[i] == '\t') {
            prefix_start = i + 1;
            break;
        }
    }
    
    // Extract prefix
    int prefix_len = pos - prefix_start;
    if (prefix_len > 0) {
        strncpy(prefix, buffer + prefix_start, prefix_len);
        prefix[prefix_len] = '\0';
    }
    
    // Determine if we're completing a command or a file
    int is_first_word = (prefix_start == 0);
    
    if (is_first_word) {
        // Complete command
        match_count = get_command_matches(prefix, matches, 50);
        
        // If no command matches, try files
        if (match_count == 0) {
            match_count = get_file_matches(prefix, matches, 50);
        }
    } else {
        // Complete file/directory
        match_count = get_file_matches(prefix, matches, 50);
    }
    
    if (match_count == 0) {
        // No matches, beep
        printf("\a");
        return pos;
    } else if (match_count == 1) {
        // Single match - complete it
        int completion_len = strlen(matches[0]);
        int chars_to_add = completion_len - prefix_len;
        
        if (pos + chars_to_add < BUFFER_SIZE - 1) {
            // Add the completion
            strcpy(buffer + prefix_start, matches[0]);
            pos = prefix_start + completion_len;
            buffer[pos] = '\0';
            
            // Redraw from prefix_start
            printf("\r%s", buffer);
        }
    } else {
        // Multiple matches - show them
        printf("\n");
        for (int i = 0; i < match_count; i++) {
            printf("%s  ", matches[i]);
            if ((i + 1) % 5 == 0) printf("\n");
        }
        if (match_count % 5 != 0) printf("\n");
        
        // Find common prefix
        int common_len = strlen(matches[0]);
        for (int i = 1; i < match_count; i++) {
            int j = 0;
            while (j < common_len && matches[i][j] == matches[0][j]) {
                j++;
            }
            common_len = j;
        }
        
        // Complete to common prefix
        if (common_len > prefix_len) {
            strncpy(buffer + prefix_start, matches[0], common_len);
            pos = prefix_start + common_len;
            buffer[pos] = '\0';
        }
        
        // Redraw prompt and buffer
        printf("myshell> %s", buffer);
    }
    
    return pos;
}

#else
// POSIX stub
int complete_input(char *buffer, int pos) {
    (void)buffer;
    (void)pos;
    printf("\a");  // Beep - not implemented
    return pos;
}
#endif

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
        } else if (ch == 9) {
            // Tab - auto-completion
            pos = complete_input(buffer, pos);
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
