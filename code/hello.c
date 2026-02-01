#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/sched.h>

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
    int res;

    if (count == 0) return 0;
    if (count > sizeof(kbuf) - 1) count = sizeof(kbuf) - 1;
    
    if (copy_from_user(kbuf, buf, count)) return -EFAULT;
    kbuf[count] = '\0';

    res = kstrtoint(kbuf, 10, &cmd);
    if (res != 0) return count;

    switch (cmd) {
        case 1:
            {
                u64 seconds = ktime_get_real_seconds();
                pr_info("MyDev: Real time seconds: %llu\n", seconds);
            }
            break;
        case 2:
            // 移除了 nr_free_pages 以兼容 GKI 符号限制
            pr_info("MyDev: Memory stats command received\n");
            break;
        case 3:
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
    .llseek = no_llseek,
};

static struct miscdevice my_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &my_fops,
};

static int __init my_init(void) {
    pr_info("MyDev: Module loaded\n");
    return misc_register(&my_misc);
}

static void __exit my_exit(void) {
    pr_info("MyDev: Module unloaded\n");
    misc_deregister(&my_misc);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Admin");
MODULE_DESCRIPTION("GKI Compatible Driver");
