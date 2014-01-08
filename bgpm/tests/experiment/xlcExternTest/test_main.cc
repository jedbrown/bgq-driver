#include <stdlib.h>
#include <stdio.h>
#include <omp.h>


extern int foobar;

extern "C" inline void printfoobar() {
    printf("foobar=%d\n", foobar);
}


int main(int argc, char *argv[])
{
    printf("Hello World\n");
    return 0;
}

