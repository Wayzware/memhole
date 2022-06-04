/*
    Wrapper for Memhole
    wrapper version: 1.4 for Memhole v1.2

    This header file allows for easy use of memhole in any C/C++ program
*/

#ifndef MEMHOLE_WRAPPER_H
#define MEMHOLE_WRAPPER_H

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>

// Error codes
#define EINVDEV 4   // invalid memhole device
#define EMEMHNF 8   // memhole device not found (/dev/memhole)
#define EMEMBSY 16  // memhole device is in use (busy)
#define EINVPID 32  // could not find pid
#define EKMALOC 64  // memhole could not kmalloc

#define LSMSPID 0   // lseek mode set pid
#define LSMSPOS 1   // lseek mode set pos
#define LSMGPOS 2   // lseek mode get pos
#define LSMSBUF 3   // lseek mode set buffer size (uninitialized at start)

#define SKMFAST 1   // address seek fast (no semaphore usage, not thread safe)
#define SKMSAFE 2   // address seek safe (blocks for semaphore, MUST be used with an IO operation with the same type to unlock sem again)
#define SKMSFNB 4   // address seek safe non-blocking (same as SKMSAFE but will not block for semaphore, instead failing if sem is locked)

#define MEMHOLE_PATH "/dev/memhole"

struct __memhole_dev_t{
    int fd;
    long buf_size;
    sem_t op_sem;
} typedef memhole_t;

// connects a memhole_t to the memhole kernel module
// note: only 1 connection is allowed at a time
// 
// returns an error code or 0 upon success
inline int connect_memhole(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    if(access(MEMHOLE_PATH, F_OK)) return -EMEMHNF;
    int ret = open(MEMHOLE_PATH, O_RDWR);
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
inline int disconnect_memhole(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd == 0) return -EINVDEV;
    return close(memhole->fd);
}

// attach memhole to pid's memory
// 
// returns an error code or 0 upon success
inline long attach_to_pid(memhole_t* memhole, int pid){
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
// mode should be an SKMxxxx macro; if you are not immediately calling read_memory or write_memory, use SKMFAST
// NOTE: the pos is NOT automatically incremented after a read/write
// 
// returns an error code or the memory address seeked to
// mode SKMSFNB will return -EMEMBSY if the semaphore could not be grabbed
inline long set_memory_position(memhole_t* memhole, void* pos, int mode){
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
inline long get_memory_position(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;

    return lseek64(memhole->fd, 0, LSMGPOS);
}

// set the memhole buffer size
// this is done automatically if a read/write call is made that would exceed the buffer size
//
// returns an error code or 0 upon success
inline long set_buffer_size(memhole_t* memhole, long len){
    if(len > memhole->buf_size){
        if(lseek64(memhole->fd, len, LSMSBUF)){
            return -EKMALOC;
        }
    }
    return 0;
}


// read memory starting at the pos set with set_memory_position()
// mode should be an SKMxxxx macro, likely equal to the one used in the last set_memory_position() call
// NOTE: the pos is NOT automatically incremented after a read/write
// 
// returns an error code or number of bytes read
inline long read_memory(memhole_t* memhole, char* buf, long len, int mode){
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
    if(mode == SKMSAFE || mode == SKMSFNB){
        sem_post(&memhole->op_sem);
    }
    return ret_val;
}

// write memory starting at the pos set with set_memory_position()
// mode should be an SKMxxxx macro, likely equal to the one used in the last set_memory_position() call
// NOTE: the pos is NOT automatically incremented after a read/write
// 
// returns an error code or number of bytes read
inline long write_memory(memhole_t* memhole, char* buf, long len, int mode){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;
    if(!((mode == SKMSAFE) | (mode == SKMFAST) | (mode == SKMSFNB))){
        printf("invalid mode used in read_memory()\n");
        return -EINVDEV;
    }
    if(set_buffer_size(memhole, len)){
        return -EKMALOC;
    }
    long ret_val = write(memhole->fd, buf, len);
    if(mode == SKMSAFE || mode == SKMSFNB){
        sem_post(&memhole->op_sem);
    }
    return ret_val;
}

#endif