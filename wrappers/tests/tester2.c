#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <alloca.h>

int main(){
    int i;
    //volatile int *x = malloc(sizeof(int));
    volatile int* x = alloca(sizeof(int) * 16384);
    
    for (i = 0; i < 16384; i++) {
        x[i] = i;
    }
    int pid = getpid();
    printf("%p\n%d\n", x, pid);

    //int fd = open("/dev/memhole", O_WRONLY); //open the device
    //lseek64(fd, x, 0); //"seek" the device to int* x

    //int res = write(fd, (char*) &num, sizeof(int)); //write num to the memory address stored on the device (currently x)
    
    for(;;);

    printf("%d\n", *x); //print the fd, the value of x (should be changed thru the write call), and the ret value of write (0 on success)
    //close(fd); //close the device
    return 0;
}