#include "../memhole.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc != 2){
        printf("usage: bptrr <pid>");
        return -1;
    }
    
    memhole_t memhole_dev = {};
    memhole_dev.fd = 0;
    if(connect_memhole(&memhole_dev)) return -1;
    long bptr = 0;
    int ret = attach_to_pid(&memhole_dev, (void*) atoi(argv[1]), &bptr);
    printf("%p\n", bptr);
    disconnect_memhole(&memhole_dev);
}