#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>        // 必须：时间函数支持
#include <linux/timekeeping.h>  // 必须：ktime_get_real_seconds
#include <linux/mm.h>           // 必须：nr_free_pages 支持
#include <linux/sched.h>       // 必须：current 指针支持

#define DEVICE_NAME "my_cool_dev"

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char msg[] = "Kernel module is active!\n";
    size_t len = sizeof(msg);
    if (*ppos >= len) return 0;
    if (count > len - *ppos) count = len - *ppos;
    if (copy_to_user(buf, msg + *ppos, count)) return -EFAULT;
    *ppos += count;
    return count;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    char kbuf[32];
    int cmd;

    if (count == 0) return 0;
    if (count > 31) count = 31;
    
    // 安全地从用户空间拷贝字符串
    if (copy_from_user(kbuf, buf, count)) return -EFAULT;
    kbuf[count] = '\0';

    // 转换数字，注意处理换行符
    if (kstrtoint(kbuf, 10, &cmd) != 0) return count;

    switch (cmd) {
        case 1:
            // 调用内核时间 API
            {
                u64 seconds = ktime_get_real_seconds();
                pr_info("MyDev: Real time seconds: %llu\n", seconds);
            }
            break;
        case 2:
            // 调用内存管理 API
            pr_info("MyDev: Free pages: %lu, approx %lu MB\n", 
                    nr_free_pages(), (nr_free_pages() << (PAGE_SHIFT - 10)) / 1024);
            break;
        case 3:
            // 访问 current 宏 (指向当前进程 task_struct)
            pr_info("MyDev: Target process: %s [%d]\n", current->comm, current->pid);
            break;
        default:
            pr_info("MyDev: Unknown CMD %d\n", cmd);
    }
    return count;
}

static const struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static struct miscdevice my_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &my_fops,
};

static int __init my_init(void) {
    return misc_register(&my_misc);
}

static void __exit my_exit(void) {
    misc_deregister(&my_misc);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
