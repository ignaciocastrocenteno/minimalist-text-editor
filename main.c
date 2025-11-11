# include <stdio.h>

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
}
