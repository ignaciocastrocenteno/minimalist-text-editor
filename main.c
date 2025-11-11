// main.c
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024

void edit_line(char* buffer, int current_line) {
    // Locate the line we want to go to in the buffer
    char *line_start = buffer;
    for (int i = 0; i < current_line; i++) {
        // Adding +1 positioning ahead of the new text line
        char *nl = strchr(line_start, '\n');
        if (!nl) {
            // Safety: requested line does not exist
            fprintf(stderr, "The requested line %d does not exist.\n", current_line);
            return;
        }
        line_start = nl + 1;
    }

    // Find the end of the current line (or end of string if it's the last line)
    char* line_end = strchr(line_start, '\n');
    if (!line_end) {
        line_end = buffer + strlen(buffer);
    }

    // --- Read new content for that line from stdin (supports spaces) ---
    // NOTE: we use a small temporary buffer to avoid overwriting tail before we shift it.
    char new_line[BUF_SIZE] = {0};
    printf("Enter the new content for that line: ");
    if (!fgets(new_line, sizeof(new_line), stdin)) {
        fprintf(stderr, "Failed to read the new line.\n");
        return;
    }
    // Strip trailing newline from fgets
    size_t nl_len = strlen(new_line);
    if (nl_len > 0 && new_line[nl_len - 1] == '\n') {
        new_line[nl_len - 1] = '\0';
        nl_len--;
    }

    // --- Compute sizes and check capacity ---
    size_t head_len = (size_t)(line_start - buffer);
    size_t old_line_len = (size_t)(line_end - line_start);
    size_t tail_len = strlen(line_end);
    size_t total_needed = head_len + nl_len + tail_len + 1; // +1 for final '\0'

    if (total_needed > BUF_SIZE) {
        // If it does not fit, we truncate the new line to the available space.
        // This keeps memory usage minimal without extra allocations.
        size_t available_for_new = BUF_SIZE - 1 - head_len - tail_len;
        if ((ssize_t)available_for_new < 0) {
            fprintf(stderr, "Not enough space to apply the change.\n");
            return;
        }
        if (nl_len > available_for_new) {
            nl_len = available_for_new;
            new_line[nl_len] = '\0';
            fprintf(stderr, "New content was truncated to fit into the buffer.\n");
        }
        total_needed = head_len + nl_len + tail_len + 1;
    }

    // --- Shift tail as needed (in-place, minimal memory) ---
    // If the new line is longer, move tail forward; if shorter, move tail backward.
    long delta = (long)nl_len - (long)old_line_len;
    if (delta != 0) {
        // memmove handles overlapping ranges safely
        memmove(line_end + delta, line_end, tail_len + 1); // +1 to move the terminating '\0'
    }

    // --- Copy new content into its final position ---
    memcpy(line_start, new_line, nl_len);
}

int main(int argc, char** argv) {
    // Reading file, using pointers and buffers
    if (argc < 2) {
        // Minimal guidance to user
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Opening text file for reading
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror("Failed to open file for reading");
        return 1;
    }

    char buffer[BUF_SIZE] = {0};
    // Read up to BUF_SIZE-1 to keep room for '\0'
    size_t read_bytes = fread(buffer, 1, BUF_SIZE - 1, f);
    buffer[read_bytes] = '\0';
    fclose(f);

    // Once the contents are in the buffer, we start the editing process
        // What line the user wants to edit
        int current_line = 0;
        printf("Contents:\n%s\n", buffer);
        printf("Enter the 0-based line number to edit: ");
        if (scanf("%d", &current_line) != 1 || current_line < 0) {
            fprintf(stderr, "Invalid line number.\n");
            return 1;
        }

        // Consume the remaining newline from scanf before fgets in edit_line
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}

        edit_line(buffer, current_line);
        
        // Opening the file again but in write mode
        f = fopen(argv[1], "w");
        if (!f) {
            perror("Failed to open file for writing");
            return 1;
        }
        
        // Getting a pointer to write to the file
        size_t to_write = strlen(buffer);
        if (fwrite(buffer, 1, to_write, f) != to_write) {
            perror("Failed to write the whole buffer");
            fclose(f);
            return 1;
        }
        fclose(f);

        return 0;
}
