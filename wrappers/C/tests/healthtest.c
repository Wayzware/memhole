#include "../memhole.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

memhole_t memhole_dev = {};

int memdmp(void *addr, long n, char *buf, long signature, long offset) {

    addr += signature;

    set_memory_position(&memhole_dev, addr);
    read_memory(&memhole_dev, buf, 8);

    addr = (void *) *((long *) buf);
    addr += offset;

    set_memory_position(&memhole_dev, addr);
    read_memory(&memhole_dev, buf, n);
}

int main(int argc, char** argv){
    void *addr;
    long n;

    if(argc != 3){
        printf("usage: memdmp <pid> <addr>\n");
        return -1;
    }
    
    memhole_dev.fd = 0;
    if(connect_memhole(&memhole_dev)) {
        printf("no connection\n");
        return -1;
    }

    if(attach_to_pid(&memhole_dev, atoi(argv[1]), NULL)){
        printf("attach error\n");
        return -1;
    }

    addr = (void *) strtol(argv[2], NULL, 16);

    char *buf = calloc(128, sizeof(char));

    memdmp(addr, 8, buf, 0x22e6650, 0x138);

    printf("Current health: %d\n", *buf);

    free(buf);
    disconnect_memhole(&memhole_dev);
}
