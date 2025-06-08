#include <stdio.h>

void hello() {
    printf("hello world\n");
    printf("hello world\n");
    printf("im new here\n");
}

void goodbye() {
    printf("im here to say goodbye\n");
}

// main entry: run alll
void run_all() {
    // register all entries
    hello();
    goodbye();
}
