#include <stdio.h>
#include <string.h>

int parse(char *name)
{
    char greeting_text[128];
    char buf[256] = {0};

    // Copy the name into the buffer
    strncpy(buf, name, sizeof(buf) - 1);

    // Add the following line to print the addresses of buf and greeting_text
    printf("%p %p\n", buf, greeting_text);

    strcpy(greeting_text, "Hello, dear ");
    strcat(greeting_text, buf);

    return 0;
}

int main()
{
    char name[256];
    printf("What's your name?\n");
    fgets(name, 256, stdin);
    parse(name);
    return 0;
}