#include "memhole.h"

int memhole_major = 250;
int memhole_minor = 0;

char* address = 0;

struct semaphore dev_sem = {};

struct semaphore status_sem = {};
int status = 0;

struct task_struct* proc_task = NULL;
int nr_pages = 0;

char* buffer = NULL;

module_param(memhole_major, int, S_IRUGO);
module_param(memhole_minor, int, S_IRUGO);

MODULE_AUTHOR("Jacob Rice");
MODULE_LICENSE("Dual BSD/GPL"); //fake news, ill sue you if you use this without my permission xd

static int memhole_open(struct inode *inode, struct file* filp){
    printkn("device opened\n");
    if(down_trylock(&dev_sem)){
        return 1;
    }
    return 0;
}

static int memhole_close(struct inode* inode, struct file* filp){
    printkn("device closed\n");
    up(&dev_sem);
    return 0;
}

static loff_t memhole_llseek(struct file* filp, loff_t addr, int m){

    if(m == LSMSPID){ //set the PID and load the data into kernel memory
        //proc_task = find_task_by_vpid((int) addr);
        proc_task = pid_task(find_vpid((int) addr), PIDTYPE_PID);
        if(proc_task == NULL){
            printkw("could not find pid");
            return 0;
        }
        return proc_task->mm->start_brk;
    }
    else if(m == LSMGPOS){
        return (loff_t) address;
    }
    else if(m == LSMSPOS){
        address = (char*) addr;
        //printk(KERN_NOTICE "MEMHOLE: seeking to: %p\n", address);
        return (loff_t) address;
    }
    else if(m == LSMSBUF){
        if(buffer){
            kfree(buffer);
        }
        buffer = kmalloc((long) addr, GFP_KERNEL);
        return !buffer;
    }

    printkw("tried to seek using an invalid mode");
    return -1;

}

static ssize_t memhole_read(struct file* filp, char __user *buf, size_t len, loff_t *f_pos){
    int bytes;
    //printk(KERN_NOTICE "MEMHOLE: reading %ld bytes from %p to %p\n", len, address, buf);
    if(!buf || !buffer) return -1;
    bytes = access_process_vm(proc_task, (unsigned long) address, (void*) buffer, len, 0);
    if(copy_to_user(buf, buffer, bytes)){
        return -1;
    }
    return bytes;
}

static ssize_t memhole_write(struct file* filp, const char __user *buf, size_t len, loff_t *f_pos){
    int bytes;
    //printk( KERN_NOTICE "MEMHOLE: writing %ld bytes from %p to %p\n", len, buf, address);
    if(!buf || !buffer) return -1;
    if(copy_from_user(buffer, buf, len)){
        return -1;
    }
    bytes = access_process_vm(proc_task, (unsigned long) address, (void*) buffer, len, FOLL_WRITE);
    return bytes;
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