#include <stdio.h>

void some_func(void) {
   printf("This is print output from some_func\n"); 
}

int main(void) {
    printf("I am output of test.c\n");
    some_func();
    return 0; // 来自 master 分支的修改
    // bugfix 第一次
    // bugfix 第二次
    // bugfix 第三次
}
