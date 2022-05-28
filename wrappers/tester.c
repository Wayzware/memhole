#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <sys/types.h>
#include <unistd.h>


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>      /* Definition of SYS_* constants */

int main(){

    int *x = 0x55e703fa72a0;
    int num = 666666;

    int fd = open("/dev/memhole", O_WRONLY); //open the device
    lseek64(fd, x, 0); //"seek" the device to int* x

    int res = write(fd, (char*) &num, sizeof(int)); //write num to the memory address stored on the device (currently x)

    //printf("%d|%d|%d\n", fd, *x, res); //print the fd, the value of x (should be changed thru the write call), and the ret value of write (0 on success)
    close(fd); //close the device
    return 0;
}