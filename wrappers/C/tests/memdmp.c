#include "../memhole.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc != 5){
        printf("usage: memdmp <pid> <addr> <len> <filename>\n");
        return -1;
    }
    
    memhole_t memhole_dev = {};
    memhole_dev.fd = 0;
    if(connect_memhole(&memhole_dev)) {
        printf("no connection\n");
        return -1;
    }
    long bptr = 0;
    int ret = attach_to_pid(&memhole_dev, atoi(argv[1]), &bptr);
    if(ret){
        printf("attach error\n");
        return -1;
    }
    char* buf = calloc(atoi(argv[3]), sizeof(char));

    set_memory_position(&memhole_dev, (void*) (strtol(argv[2], NULL, 16)));
    int bytes = read_memory(&memhole_dev, buf, atoi(argv[3]));
    FILE* fi = fopen64(argv[4], "w+b");
    fwrite(buf, bytes, 1, fi);
    fclose(fi);
    free(buf);
    disconnect_memhole(&memhole_dev);
}