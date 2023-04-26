#include <unistd.h>
#include <stdio.h>

int main() {

    int seconds = alarm(5);
    printf("seconds = %d\n", seconds);

    sleep(2);
    seconds = alarm(6);
    printf("seconds = %d\n", seconds);

    while(1) ;

    return 0;
}