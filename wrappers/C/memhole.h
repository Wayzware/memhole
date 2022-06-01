/*
    Wrapper for Memhole
    wrapper version: 1.2 for Memhole v1.0 and 1.1

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

// Error codes
#define EINVDEV 4   // invalid memhole device
#define EMEMHNF 8   // memhole device not found (/dev/memhole)
#define EMEMBSY 16  // memhole device is in use (busy)
#define EINVPID 32  // could not find pid

#define LSMSPID 0   // lseek mode set pid
#define LSMSPOS 3   // lseek mode set pos
#define LSMGPOS 4   // lseek mode get pos

#define MEMHOLE_PATH "/dev/memhole"

struct __memhole_dev_t{
    int fd;
} typedef memhole_t;

// connects a memhole_t to the memhole kernel module
// note: only 1 connection is allowed at a time
// returns an error code or 0 upon success
inline int connect_memhole(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    if(access(MEMHOLE_PATH, F_OK)) return -EMEMHNF;
    int ret = open(MEMHOLE_PATH, O_RDWR);
    if(ret < 0){
        return -EMEMBSY;
    }
    memhole->fd = ret;
    return 0;
}

// disconnects a memhole_t from the memhole kernel module
// returns an error code or 0 upon success
inline int disconnect_memhole(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd == 0) return -EINVDEV;
    return close(memhole->fd);
}

// attach memhole to pid's memory
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
// NOTE: the pos is NOT automatically incremented after a read/write
// returns an error code or the memory address seeked to
inline long set_memory_position(memhole_t* memhole, void* pos){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;
    return lseek64(memhole->fd, (long) pos, LSMSPOS);
}

// get the memory position currently used for reading/writing from
// returns an error code or the memory address requested
inline long get_memory_position(memhole_t* memhole){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;

    return lseek64(memhole->fd, 0, LSMGPOS);
}

// read memory starting at the pos set with set_memory_position()
// NOTE: the pos is NOT automatically incremented after a read/write
// returns an error code or number of bytes read
inline long read_memory(memhole_t* memhole, char* buf, long len){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;

    return read(memhole->fd, buf, len);
}

// write memory starting at the pos set with set_memory_position()
// NOTE: the pos is NOT automatically incremented after a read/write
// returns an error code or number of bytes read
inline long write_memory(memhole_t* memhole, char* buf, long len){
    if(memhole == NULL) return -EINVDEV;
    if(memhole->fd <= 0) return -EINVDEV;

    return write(memhole->fd, buf, len);
}

#endif