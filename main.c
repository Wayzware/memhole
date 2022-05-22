#include "memhole.h"

int memhole_major = 250;
int memhole_minor = 0;

void* address = 0;

struct semaphore dev_sem = {};

module_param(memhole_major, int, S_IRUGO);
module_param(memhole_minor, int, S_IRUGO);

MODULE_AUTHOR("Jacob Rice");
MODULE_LICENSE("All Rights Reserved");

static int memhole_open(struct inode *inode, struct file* filp){
    printkn("opened\n");
    if(down_trylock(&dev_sem)){
        return 1;
    }
    return 0;
}

static int memhole_close(struct inode* inode, struct file* filp){
    printkn("closed\n");
    up(&dev_sem);
    return 0;
}

static loff_t memhole_llseek(struct file* filp, loff_t addr, int m){
    address = (void*) addr;
    printk(KERN_NOTICE "MEMHOLE: seeking to: %d\n", (int)address);
    return (loff_t) address;
}

static ssize_t memhole_read(struct file* filp, char __user *buf, size_t len, loff_t *f_pos){
    unsigned long ret_val = copy_to_user(buf, address, len);
    address += len - ret_val;
    if(ret_val){
        return -EFAULT;
    }
    return 0;
}

static ssize_t memhole_write(struct file* filp, const char __user *buf, size_t len, loff_t *f_pos){
    printk( KERN_NOTICE "MEMHOLE: writing %ld bytes to %ld\n", len, (long) buf);
    unsigned long ret_val = copy_from_user(address, buf, len);
    address += len - ret_val;
    if(ret_val){
        return -EFAULT;
    }
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
    printkn("init begin\n");
    sema_init(&dev_sem, 1);

    memhole_major = register_chrdev(0, "memhole", &memhole_fops);
    if(memhole_major < 0){
        printke("Failed to register kernel module\n");
        printk( KERN_EMERG "%d", memhole_major);
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