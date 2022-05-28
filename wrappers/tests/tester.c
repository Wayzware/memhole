#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <sys/types.h>
#include <unistd.h>


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv){

    if(argc != 4){
        printf("You forgor the arguments dumbass\n");
        return 0;
    }

    int *x = (int *) strtol(argv[1], NULL, 16);
    int pid = atoi(argv[2]);
    int num = atoi(argv[3]);

    int fd = open("/dev/memhole", O_WRONLY); //open the device
    if(lseek64(fd, pid, 0)){
        printf("uh oh");
        return 0;
    }
    if(lseek64(fd, x, 3) != x){ //"seek" the device to int* x
        printf("uh oh 2");
        return 0;
    }
    int res = write(fd, (char*) &num, sizeof(int)); //write num to the memory address stored on the device (currently x)

    //printf("%d|%d|%d\n", fd, *x, res); //print the fd, the value of x (should be changed thru the write call), and the ret value of write (0 on success)
    close(fd); //close the device
    return 0;
}