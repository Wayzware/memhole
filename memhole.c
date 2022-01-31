//#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int memhole_main(void)
{
    printk();
}

void memhole_exit(void)
{

}

module_init(memhole_main);
module_exit(memhole_exit);