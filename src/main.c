#include <stdio.h>

// single file detection
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stdout, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // read file binary
    FILE* file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stdout, "Failed opening file"); 
    }

    size_t last_size = 0;

    fseek(file, 0, SEEK_END);
    long init_size = ftell(file);

    while(1) {

        

    }

    fclose(file);
    return 0;
}
