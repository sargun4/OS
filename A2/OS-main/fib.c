#include <stdio.h>
#include <stdlib.h>
int fib(int n) {
  if (n < 2) return n;
  else return fib(n - 1) + fib(n - 2);
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1; //error status code
    }
    int n = atoi(argv[1]); // converting argument to an integer
    int result = fib(n);
    printf("Fibonacci number for n=%d is %d\n", n, result);
    return 0;
}
