#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <sys/types.h>
#include <unistd.h>


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>      /* Definition of SYS_* constants */

int main(){

    int *x = malloc(sizeof(int));
    int num = 6;

    int fd = open("/dev/memhole", O_WRONLY);
    lseek64(fd, x, 0);
    int res = write(fd, (char*) &num, sizeof(int));
    printf("%d|%d|%d\n", fd, *x, res);
    close(fd);
    return 0;
}