#include <stdio.h>

void some_func(void) {
   printf("This is print output from some_func\n"); 
}

int main(void) {
    printf("I am output of test.c\n");
    some_func();
    return 0;
}
