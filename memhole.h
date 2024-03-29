#include <linux/sched.h>

#include <linux/configfs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/vmalloc.h>    /* vmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <linux/pagemap.h>
#include <linux/mm.h>

#include <linux/uaccess.h>	/* copy_*_user */
#include <linux/sched/signal.h>

#include <linux/types.h>	/* size_t */

#include <asm/smap.h>

#define MEMHOLE_VERSION "1.3.0-Full"

//#define MEMHOLE_DEBUG

#define LSMSPID 0
#define LSMSPOS 1
#define LSMGPOS 2
#define LSMSBUF 3

#define printkn(x) printk( KERN_NOTICE "MEMHOLE: %s", x)
#define printkw(x) printk( KERN_WARNING "MEMHOLE: %s", x)
#define printke(x) printk( KERN_EMERG "MEMHOLE: %s", x)
