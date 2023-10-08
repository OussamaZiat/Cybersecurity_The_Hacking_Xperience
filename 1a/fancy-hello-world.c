#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "fancy-hello-world.h"

int main() {

    char name[100];

    printf("Enter your name (max length = 100): ");
    /*
    fgets(name, 256, stdin);
    printf("%s", name); */
    scanf("%100s", name);

    char hello_world[256] = "Hello World, hello ";
    char *output = hello_world;

    strcpy(output, hello_world );
    hello_string(name, output);


    return 0;
}

void hello_string(char *name, char *output) {
    strcat(output, name);
    printf("%s\n", output);
}