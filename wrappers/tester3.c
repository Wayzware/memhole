#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <sys/types.h>
#include <unistd.h>


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv){
    int i;

    if(argc != 3){
        printf("You forgor the arguments dumbass\n");
        return 0;
    }

    int *x = (int *) strtol(argv[1], NULL, 16);
    int pid = atoi(argv[2]);
    
    FILE *file = fopen("log.txt", "w+");

    int fd = open("/dev/memhole", O_RDONLY); //open the device
    if(lseek64(fd, pid, 0)){
        printf("uh oh");
        return 0;
    }
    if(lseek64(fd, x, 3) != x){ //"seek" the device to int* x
        printf("uh oh 2");
        return 0;
    }

    int buf[16384] = {};

    
    int res = read(fd, (char*) buf, sizeof(int)*16384); //write num to the memory address stored on the device (currently x)

    for (i = 0; i < 16384; i++) {
        fprintf(file, "%d\n", buf[i]);
    }

    close(fd); //close the device

    fclose(file);
    return 0;
}