#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd9726f80, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x4caf37f7, "param_ops_int" },
	{ 0x5f3da946, "find_vpid" },
	{ 0x4102c7cb, "__register_chrdev" },
	{ 0xe9ffc063, "down_trylock" },
	{ 0xc5850110, "printk" },
	{ 0x53377cb8, "pid_task" },
	{ 0x2d6e1040, "access_process_vm" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xcf2a6966, "up" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "A3152A164071290D5483109");
