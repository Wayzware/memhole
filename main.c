#include "memhole.h"

#define MEMHOLE_VERSION "0.2.1"

int memhole_major = 250;
int memhole_minor = 0;

char* address = 0;

struct semaphore dev_sem = {};

struct semaphore status_sem = {};
int status = 0;

int usr_proc_pid = 0;

module_param(memhole_major, int, S_IRUGO);
module_param(memhole_minor, int, S_IRUGO);

MODULE_AUTHOR("Jacob Rice");
MODULE_LICENSE("All Rights Reserved");

static int memhole_open(struct inode *inode, struct file* filp){
    printkn("device opened\n");
    if(down_trylock(&dev_sem)){
        return 1;
    }
    return 0;
}
//123652
static int memhole_close(struct inode* inode, struct file* filp){
    printkn("device closed\n");
    up(&dev_sem);
    return 0;
}

static loff_t memhole_llseek(struct file* filp, loff_t addr, int m){

    if(m == SEEK_SET){ //set the PID and load the data into kernel memory
        struct pid pid_struct = {};
        pid_struct.
        kill_pid()
    }
    else if(m == SEEK_DATA){
        address = (char*) addr;
        printk(KERN_NOTICE "MEMHOLE: seeking to: %p\n", address);
        return (loff_t) address;
    }

    printkw("tried to seek using an invalid mode");
    return -1;

}

static ssize_t memhole_read(struct file* filp, char __user *buf, size_t len, loff_t *f_pos){
    stac();
    unsigned long ret_val = copy_to_user(buf, address, len);
    clac();
    address += len - ret_val;
    if(ret_val){
        return -EFAULT;
    }
    return 0;
}

static ssize_t memhole_write(struct file* filp, const char __user *buf, size_t len, loff_t *f_pos){
    size_t x;
    printk( KERN_NOTICE "MEMHOLE: writing %ld bytes to %p\n", len, buf);
    stac();
    for(x = 0; x < len; x++){
        *address = *(buf + x);
        address++;
    }
    clac();
    return 0;
}

const struct file_operations memhole_fops = {
    .owner = THIS_MODULE,
    .llseek = memhole_llseek,
    .read = memhole_read,
    .write = memhole_write,
    .open = memhole_open,
    .release = memhole_close,
};

int memhole_init(void){
    printk(KERN_NOTICE "MEMHOLE: init version %s", MEMHOLE_VERSION);
    //printkn("init begin\n");
    sema_init(&dev_sem, 1);
    sema_init(&status_sem, 1);

    memhole_major = register_chrdev(0, "memhole", &memhole_fops);
    if(memhole_major < 0){
        printke("Failed to register kernel module\n");
        printk( KERN_EMERG "MEMHOLE: %d", memhole_major);
        return memhole_major;
    }

    printkn("init completed\n");
    return 0;
}

void memhole_exit(void){
    if(memhole_major > 0){
        unregister_chrdev(memhole_major, "memhole");
    }
    printkn("exit\n");
}

module_init(memhole_init);
module_exit(memhole_exit);