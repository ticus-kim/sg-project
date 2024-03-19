#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
    if(argc != 5) return -1;
    int fibo = fibonacci(atoi(argv[1]));
    int mofi = max_of_four_int(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    printf("%d %d\n",fibo, mofi);

    return 1;
}
