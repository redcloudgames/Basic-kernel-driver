#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include "include/basic_kernel_driver.h"

#define DEVICE_NAME "memdev"
#define BUF_LEN 1024

static int major_number;
static char *kernel_buffer;
static struct cdev memdev_cdev;
static DEFINE_MUTEX(dev_mutex); // For thread safety

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,
    .mmap = dev_mmap, // Memory-mapping support
};

// Open function
static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "memdev: Device opened\n");
    return 0;
}

// Release function
static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "memdev: Device successfully closed\n");
    return 0;
}

// Read function
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    if (*offset >= BUF_LEN) return 0;

    // Non-blocking I/O support
    if (filep->f_flags & O_NONBLOCK && *offset == 0)
        return -EAGAIN;

    len = (len + *offset > BUF_LEN) ? (BUF_LEN - *offset) : len;

    if (copy_to_user(buffer, kernel_buffer + *offset, len))
        return -EFAULT;

    *offset += len;
    printk(KERN_INFO "memdev: Sent %zu characters to the user\n", len);
    return len;
}

// Write function
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    mutex_lock(&dev_mutex);

    len = (len > BUF_LEN) ? BUF_LEN : len;
    if (copy_from_user(kernel_buffer, buffer, len)) {
        mutex_unlock(&dev_mutex);
        return -EFAULT;
    }

    printk(KERN_INFO "memdev: Received %zu characters from the user\n", len);
    mutex_unlock(&dev_mutex);
    return len;
}

// IOCTL function (for extended operations)
static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
    int new_size;
    char *new_buffer;

    switch (cmd) {
        case IOCTL_SET_BUFFER_SIZE:
            if (copy_from_user(&new_size, (int __user *)arg, sizeof(new_size)))
                return -EFAULT;

            if (new_size <= 0)
                return -EINVAL;

            new_buffer = krealloc(kernel_buffer, new_size, GFP_KERNEL);
            if (!new_buffer)
                return -ENOMEM;

            kernel_buffer = new_buffer;
            BUF_LEN = new_size;
            printk(KERN_INFO "memdev: Resized buffer to %d bytes\n", new_size);
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

// Memory-mapping function
static int dev_mmap(struct file *filep, struct vm_area_struct *vma) {
    return remap_pfn_range(vma, vma->vm_start,
                           virt_to_phys((void *)kernel_buffer) >> PAGE_SHIFT,
                           vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

// Module initialization
static int __init memdev_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "memdev: Failed to register a major number\n");
        return major_number;
    }

    kernel_buffer = kmalloc(BUF_LEN, GFP_KERNEL);
    if (!kernel_buffer) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "memdev: Failed to allocate memory\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "memdev: Module loaded\n");
    return 0;
}

// Module exit
static void __exit memdev_exit(void) {
    kfree(kernel_buffer);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "memdev: Module unloaded\n");
}

module_init(memdev_init);
module_exit(memdev_exit);

MODULE_LICENSE("GPL3.1");
MODULE_AUTHOR("XQuzmi");
MODULE_DESCRIPTION("Basic Kernel Driver with Read/Write Memory Functions and Additional Features");
MODULE_VERSION("0.1");
