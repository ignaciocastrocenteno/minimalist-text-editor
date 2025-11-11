#include <stdio.h>
#include <string.h>

void main(int argc, char** argv) {
    // Reading file, using pointers and buffers
    // Opening text file for reading
    FILE* f = fopen(argv[1], "r");
    char buffer[1024] = {0};
    fread(buffer, 1024, 1, f);
    fclose(f);

    // Once the contents are in the buffer, we start the editing process
        // What line the user wants to edit
        int current_line = 0;
        printf("Contents:\n%s\n", buffer);
        scanf("%d", &current_line);
        edit_line(current_line, buffer)
        
        // Opening the file again but in write mode
        f - fopen(argv[1], "w");
        
        // Getting a pointer to write to the file
        fwrite(buffer, strlen(buffer), 1, f);
        fclose(f);
}

void edit_line(char* buffer, int current_line) {
    // Locate the line we want to go to in the buffer
    for (int i = 0; i < current_line; i++) {
        // Adding +1 positioning ahead of the new text line
        buffer = strchr(buffer, '\n') + 1;
    }

    char* line_end = strchr(buffer, '\n');
    char saved[1024] = {0};
    // Copying the line end
    strcpy(saved, line_end);
    scanf("%s", buffer);
    // Getting the length of the text line, the user entered
    strcpy(buffer + strlen(buffer), saved);
}
