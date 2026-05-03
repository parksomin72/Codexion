#include <stdio.h>
#include <string.h>

void test(char *const p) {
     strcpy(p, "Hello World!");
     printf("%s\n", p);
}

int main() {
    char str[100] = "Hello";
    char *const p = str;
    test(p);
}
