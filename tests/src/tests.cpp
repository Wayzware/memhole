#include "../../wrappers/C/memhole.h"
#include "timer.h"

int main(int argc, char** argv) {
    if(argc != 5) {
        printf("Usage: test.o <pid> <addr> <read len> <iterations>");
        return 0;
    }
    memhole_t* memhole = create_memhole();
    printf("connect: %d\n", connect_memhole(memhole));
    printf("attach to pid: %ld\n", attach_to_pid(memhole, atoi(argv[1])));
    printf("set mem: %p\n", (void*)set_memory_position(memhole, (void*) atol(argv[2]), SKMFAST));
    printf("buffer size: %ld\n", set_buffer_size(memhole, atol(argv[3])));
    void* buffer = malloc(atol(argv[3]));
    printf("buffer addr: %p\n", buffer);
    Timer timer;
    long len = atol(argv[3]);
    timer.reset();
    long total = 0;
    long tests = atol(argv[4]);
    for(int i = 0; i < tests; i++){
        read_memory(memhole, (char*)buffer, len, SKMFAST);
        total += timer.get_us_reset();
    }

    printf("\navg time: %ld ns\n", total / (tests));
    printf("speed: %ld MB/s\n", len / (total / tests));

    printf("\n");
    printf("disconnect: %d\n", disconnect_memhole(memhole));
    delete_memhole(memhole);
    return 0;
}