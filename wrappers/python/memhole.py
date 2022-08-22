# Python wrapper v1.0 for Memhole v1.2.x
# This file allows for easy use of memhole in any python program
# The memhole kernel module must be loaded for these functions to work properly

# note: unlike the v1.6+ C/C++ wrapper, this wrapper has no option
# for thread safety between io operations; it is not thread safe

import os

MEMHOLE_PATH = "/dev/memhole"

#error codes
EINVDEV = 4
EMEMHNF = 8
EMEMBSY = 16
EINVPID = 32
EKMALOC = 64


class Memhole():
    buf_size = 0
    write_perms = False
    fd = 0

    __LSMSPID = 0
    __LSMSPOS = 1
    __LSMGPOS = 2
    __LSMSBUF = 3

    def __init__(self, write_perms : bool):
        self.mode = mode
        self.write_perms = write_perms
    
    #returns 0 on success or an error code
    def connect(self) -> int:
        if self.fd != 0:
            return -EINVDEV
        mode = os.O_RDONLY
        if self.write_perms:
            mode = os.O_RDWR
        if not os.access(MEMHOLE_PATH, os.F_OK):
            return -EMEMHNF
        self.fd = os.open(MEMHOLE_PATH, mode)
        if self.fd < 1:
            self.fd = 0
            return -EMEMBSY
        self.buf_size = 0
        return 0
    
    #returns 0 on success or an error code
    def disconnect(self) -> int:
        if self.fd == 0:
            return -EINVDEV
        ret_val = os.close(self.fd)
        if ret_val != 0:
            return ret_val
        self.fd = 0
        return 0
    
    #returns 0 on success or an error code
    def attach_to_pid(self, pid):
        if self.fd == 0: return -EINVDEV
        if os.kill(pid, 0): return -EINVPID

        ret = os.lseek(self.fd, pid, self.__LSMSPID)
        if ret == 0: return -EINVPID
        return 0
    
    #returns 0 on success or an error code
    def set_memory_position(self, pos):
        if self.fd == 0: return -EINVDEV
        return os.lseek(self.fd, pos, self.__LSMSPOS)

    #returns current memory address on success or an error code
    def get_memory_position(self):
        if self.fd == 0: return -EINVDEV
        return os.lseek(self.fd, 0, self.__LSMGPOS)
    
    #returns 0 on success or an error code
    def set_buffer_size(self, len):
        if self.fd == 0: return -EINVDEV
        if(len < self.buf_size):
            if(os.lseek(self.fd, len, self.__LSMSBUF) != 0):
                return -EKMALOC
        return 0

    #returns a bytestring
    def read_memory(self, len):
        if self.fd == 0: return -EINVDEV
        if(self.set_buffer_size(len) != 0): return -EKMALOC
        return os.read(self.fd, len)
    
    #returns the number of bytes written
    def write_memory(self, buf_str, len):
        if self.fd == 0: return -EINVDEV
        if(self.set_buffer_size(len) != 0): return -EKMALOC
        return os.write(self.fd, buf_str)
