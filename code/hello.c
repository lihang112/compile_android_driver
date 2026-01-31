#include <linux/module.h>    // 所有模块都需要这个头文件
#include <linux/kernel.h>    // KERN_INFO 级别定义
#include <linux/init.h>      // 宏定义 __init 和 __exit

static int __init hello_android_init(void)
{
    printk(KERN_INFO "Hello Android: Kernel Driver loaded successfully!\n");
    return 0; // 返回 0 表示加载成功
}

static void __exit hello_android_exit(void)
{
    printk(KERN_INFO "Hello Android: Goodbye, kernel world!\n");
}

module_init(hello_android_init);
module_exit(hello_android_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("A simple Hello World driver for Android GKI");
