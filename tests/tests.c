#ifndef MEMHOLE_WRAPPER_H
#define MEMHOLE_WRAPPER_H
/*
    Wrapper for Memhole
    wrapper v1.7 for Memhole v1.3.x

    This header file allows for easy use of memhole in any C/C++ program
    The memhole kernel module must be loaded for these functions to work properly
*/

/*
    To use:
    create a new memhole_t* by calling create_memhole()
    call connect_memhole() with a pointer to your memhole_t as the argument 
    use attach_to_pid() to attach memhole to a process id's memory
    use set_memory_position() to set the position in memory to read/write from
    call read_memory() or write_memory() to read/write memory
    when done, delete the memhole_t with delete_memhole()
    NOTE: write_memory() only works if the macro on line 28 has been uncommented (for safety)
    NOTE: the modes used in set_memory_position() and read/write_memory() are 
*/

// NOTE: writing is disabled by default for safety
//
// if your version of memhole supports writing and you need to write,
// you can uncomment this line to allow writing with memhole
//#define MEMHOLEW_ALLOW_WRITE

// if for some reason you want memhole to be open after an exec() family call,
// comment out the line below
#define MEMHOLEW_CLOSE_ON_EXEC

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// Error codes
#define EINVDEV 4   // invalid memhole device
#define EMEMHNF 8   // memhole device not found (/dev/memhole)
#define EMEMBSY 16  // memhole device is in use (busy)
#define EINVPID 32  // could not find pid
#define EKMALOC 64  // memhole could not kmalloc

// internal IO operation definitions (use memhole_mode_t for mode arguments)
#define _SKMFAST 1
#define _SKMSAFE 2
#define _SKMSFNB 4


// IO operation modes
//
// in general:
// if your program accesses memhole on more than 1 thread, use a safe mode
// if your program has only 1 thread, SKMFAST will be safe to use
typedef enum{
    SKMFAST = _SKMFAST, // address seek fast (no semaphore usage, not thread safe)
    SKMSAFE = _SKMSAFE, // address seek safe (blocks for semaphore, MUST be used with an IO operation with the same type to unlock sem again)
    SKMSFNB = _SKMSFNB  // address seek safe non-blocking (same as SKMSAFE but will not block for semaphore, instead failing if sem is locked)
} memhole_mode_t;

// internal memhole modes, DO NOT USE in wrapper function arguments
#define LSMSPID 0   // lseek mode set pid
#define LSMSPOS 1   // lseek mode set pos
#define LSMGPOS 2   // lseek mode get pos
#define LSMSLEN 3   // lseek mode set buff length
#define LSMGBUF 4   // lseek mode get buffer address

#define MEMHOLE_PATH "/dev/memhole"

// do not manually create or modify the values in a memhole_t
// instead, use create_memhole() to make a new memhole_t
// use delete_memhole() to free
struct __memhole_dev_t{
    int fd;
    long buf_size;
    sem_t op_sem;
} typedef memhole_t;


// initializes a memhole_t struct
// note: this function does not connect to the memhole device (use connect_memhole() after this function)
//
// returns a pointer to a new memhole_t or 0 upon malloc error
 memhole_t* create_memhole(){
    memhole_t* ret_val = (memhole_t*) malloc(sizeof(memhole_t));
    if(!ret_val) return 0;
    ret_val->fd = 0;
    ret_val->buf_size = 0;
    //ret_val->op_sem = 0;
    return ret_val;
}

// frees a memhole_t
// note: this function does not disconnect from the memhole device (use disconnect_memhole() before this function)
//
// returns an error code or 0 upon success
 int delete_memhole(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    free(memhole);
    return 0;
}

// connects a memhole_t to the memhole kernel module
// note: only 1 connection is allowed at a time
// 
// returns an error code or 0 upon success
 int connect_memhole(memhole_t* memhole){
    #ifdef MEMHOLEW_ALLOW_WRITE
    int mode = O_RDWR;
    #endif
    #ifndef MEMHOLEW_ALLOW_WRITE
    int mode = O_RDONLY;
    #endif
    #ifdef MEMHOLEW_CLOSE_ON_EXEC
    mode = mode | O_CLOEXEC;
    #endif

    if(memhole == NULL) return -EINVDEV;
    if(access(MEMHOLE_PATH, F_OK)) return -EMEMHNF;

    int ret = open(MEMHOLE_PATH, mode);

    if(ret < 0){
        return -EMEMBSY;
    }
    memhole->fd = ret;
    memhole->buf_size = -1;
    sem_init(&memhole->op_sem, 0, 1);
    return 0;
}

// disconnects a memhole_t from the memhole kernel module
// 
// returns an error code or 0 upon success
 int disconnect_memhole(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd == 0) return -EINVDEV;
    return close(memhole->fd);
}

// attach memhole to pid's memory
// 
// returns an error code or 0 upon success
 long attach_to_pid(memhole_t* memhole, int pid){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;
    if(kill(pid, 0)) return -EINVPID;

    long ret = lseek64(memhole->fd, pid, LSMSPID);
    if(ret == 0){
        return -EINVPID;
    }
    return 0;
}

// set the memory position to start reading/writing from
// mode should be a memhole_mode_t enum; if you are not immediately calling read_memory or write_memory, use SKMFAST
// NOTE: the pos is NOT automatically incremented after a read/write
// 
// returns an error code or the memory address seeked to
// mode SKMSFNB will return -EMEMBSY if the semaphore could not be grabbed
 long set_memory_position(memhole_t* memhole, void* pos, memhole_mode_t mode){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;
    if(mode == SKMSAFE){
        sem_wait(&memhole->op_sem);
    }
    else if(mode == SKMSFNB){
        if(sem_trywait(&memhole->op_sem)){
            return -EMEMBSY;
        }
    }
    else if(mode != SKMFAST){
        printf("invalid mode used for set_memory_position()\n");
        return -EINVDEV;
    }
    return lseek64(memhole->fd, (long) pos, LSMSPOS);
}

// get the memory position currently used for reading/writing from
// 
// returns an error code or the memory address requested
 long get_memory_position(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;

    return lseek64(memhole->fd, 0, LSMGPOS);
}

// set the memhole buffer size
// this is done automatically if a read/write call is made that would exceed the buffer size
//
// returns an error code or 0 upon success
 long set_buffer_size(memhole_t* memhole, long len){
    if(len > memhole->buf_size){
        if(lseek64(memhole->fd, len, LSMSLEN)){
            return -EKMALOC;
        }
    }
    return 0;
}

// get the address of the memhole shared user/kernel buffer
//
// returns the address of the buffer or 0 if it has not yet been created
 long get_buffer_addr(memhole_t* memhole){
    return lseek64(memhole->fd, 0, LSMGBUF);
}

// read memory starting at the pos set with set_memory_position()
// mode should be a memhole_mode_t enum, likely equal to the one used in the last set_memory_position() call
// NOTE: the pos is NOT automatically incremented after a read/write
// 
// returns an error code or number of bytes read
 long read_memory(memhole_t* memhole, char* buf, long len, memhole_mode_t mode){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;
    if(!((mode == SKMSAFE) | (mode == SKMFAST) | (mode == SKMSFNB))){
        printf("invalid mode used in read_memory()\n");
        return -EINVDEV;
    }
    if(set_buffer_size(memhole, len)){
        return -EKMALOC;
    }
    long ret_val = read(memhole->fd, buf, len);
    if(mode & (SKMSAFE | SKMSFNB)){
        sem_post(&memhole->op_sem);
    }
    return ret_val;
}

#ifdef MEMHOLEW_ALLOW_WRITE
// write memory starting at the pos set with set_memory_position()
// mode should be a memhole_mode_t enum, likely equal to the one used in the last set_memory_position() call
// NOTE: the pos is NOT automatically incremented after a read/write
// 
// returns an error code or number of bytes written
 long write_memory(memhole_t* memhole, char* buf, long len, memhole_mode_t mode){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;
    if(!((mode == SKMSAFE) | (mode == SKMFAST) | (mode == SKMSFNB))){
        printf("invalid mode used in write_memory()\n");
        return -EINVDEV;
    }
    if(set_buffer_size(memhole, len)){
        return -EKMALOC;
    }
    long ret_val = write(memhole->fd, buf, len);
    if(mode & (SKMSAFE | SKMSFNB)){
        sem_post(&memhole->op_sem);
    }
    return ret_val;
}
#endif

#endif


int main(int argc, char** argv) {
    memhole_t* memhole = create_memhole();
    printf("connect: %d\n", connect_memhole(memhole));
    printf("attach to pid: %ld\n", attach_to_pid(memhole, atoi(argv[1])));
    printf("set mem: %p\n", (void*)set_memory_position(memhole, (void*) atol(argv[2]), SKMFAST));
    printf("buffer size: %ld\n", set_buffer_size(memhole, atol(argv[3])));
    void* buffer = (void*) get_buffer_addr(memhole);
    printf("buffer addr: %p\n", buffer);
    printf("%ld\n", read_memory(memhole, buffer, atol(argv[3]), SKMFAST));
    for(int i = 0; i < atol(argv[3]); i++) {
        printf("%c", ((char*) buffer)[i]);
    }
    printf("\n");
    printf("disconnect: %d\n", disconnect_memhole(memhole));
    delete_memhole(memhole);
    return 0;
}