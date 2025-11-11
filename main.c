# include <stdio.h>

void main(int argc, char** argv) {
    FILE* f = fopen(argv[1], "r");
    char buffer[1024] = {0};
    fread(buffer, 1024, 1, f);
    fclose(f);
    
}
