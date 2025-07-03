#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include "../include/xor_encrypt.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grupo-06");
MODULE_DESCRIPTION("Chardevice con cifrado XOR");
MODULE_VERSION("1.0");

/* Variables estáticas del módulo */
static int major;
static struct xor_device {
    struct cdev cdev;
    struct class *class;
    struct device *device;
    char buffer[MAX_SIZE];
    size_t data_size;
    char xor_key;
    struct mutex lock;
} xor_dev;

/* Inicialización del mutex */
static DEFINE_MUTEX(xor_mutex);

static int dev_open(struct inode *inode, struct file *file) {
    if (!mutex_trylock(&xor_mutex)) return -EBUSY;
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    mutex_unlock(&xor_mutex);
    return 0;
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t len, loff_t *off) {
    size_t i;

    if (len > MAX_SIZE) return -EINVAL;
    if (copy_from_user(buffer, user_buf, len)) return -EFAULT;

    for (i = 0; i < len; i++)
        buffer[i] ^= xor_key;

    data_size = len;
    return len;
}

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t len, loff_t *off) {
    size_t to_copy;
    char temp[MAX_SIZE];
    size_t i;

    if (*off >= data_size) return 0;

    to_copy = min(len, data_size - *off);

    for (i = 0; i < to_copy; i++)
        temp[i] = buffer[*off + i] ^ xor_key;

    if (copy_to_user(user_buf, temp, to_copy)) return -EFAULT;

    *off += to_copy;
    return to_copy;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    char new_key;
    if (cmd == IOCTL_SET_KEY) {
        if (copy_from_user(&new_key, (char __user *)arg, sizeof(char))) return -EFAULT;
        xor_key = new_key;
        return 0;
    }
    return -EINVAL;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl,
};

static int __init xor_init(void) {
    dev_t dev;
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret) return ret;

    major = MAJOR(dev);
    cdev_init(&xor_cdev, &fops);
    cdev_add(&xor_cdev, dev, 1);

    xor_class = class_create(CLASS_NAME);
    xor_device = device_create(xor_class, NULL, dev, NULL, DEVICE_NAME);

    mutex_init(&xor_mutex);
    printk(KERN_INFO "xor_encrypt: Module loaded, major %d\n", major);
    return 0;
}

static void __exit xor_exit(void) {
    dev_t dev = MKDEV(major, 0);
    device_destroy(xor_class, dev);
    class_destroy(xor_class);
    cdev_del(&xor_cdev);
    unregister_chrdev_region(dev, 1);
    mutex_destroy(&xor_mutex);
    printk(KERN_INFO "xor_encrypt: Module unloaded\n");
}

module_init(xor_init);
module_exit(xor_exit);