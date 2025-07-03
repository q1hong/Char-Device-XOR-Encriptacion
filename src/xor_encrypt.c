#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include "../include/xor_encrypt.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grupo-06");
MODULE_DESCRIPTION("Dispositivo de caracteres con cifrado XOR");
MODULE_VERSION("1.0");

/* Variables estáticas del módulo */
static int major;
static struct xor_device xor_dev;

/* Inicialización del mutex para sincronización */
static DEFINE_MUTEX(xor_mutex);

static int dev_open(struct inode *inode, struct file *file) {
    struct xor_device *dev = container_of(inode->i_cdev, struct xor_device, cdev);
    file->private_data = dev;
    if (!mutex_trylock(&dev->lock)) return -EBUSY;
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    struct xor_device *dev = file->private_data;
    mutex_unlock(&dev->lock);
    return 0;
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t len, loff_t *off) {
    size_t i;
    struct xor_device *dev = &xor_dev;

    if (len > MAX_SIZE) return -EINVAL;  /* Validar tamaño máximo */
    if (copy_from_user(dev->buffer, user_buf, len)) return -EFAULT;  /* Copiar datos del espacio de usuario */

    for (i = 0; i < len; i++)
        dev->buffer[i] ^= dev->xor_key;

    dev->data_size = len;
    return len;
}

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t len, loff_t *off) {
    size_t to_copy;
    char temp[MAX_SIZE];
    size_t i;
    struct xor_device *dev = &xor_dev;

    if (*off >= dev->data_size) return 0;

    to_copy = min(len, dev->data_size - *off);

    for (i = 0; i < to_copy; i++)
        temp[i] = dev->buffer[*off + i] ^ dev->xor_key;

    if (copy_to_user(user_buf, temp, to_copy)) return -EFAULT;

    *off += to_copy;
    return to_copy;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    char new_key;
    struct xor_device *dev = &xor_dev;
    
    if (cmd == IOCTL_SET_KEY) {
        if (copy_from_user(&new_key, (char __user *)arg, sizeof(char))) return -EFAULT;
        dev->xor_key = new_key;
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
    
    /* Inicializar la estructura del dispositivo */
    cdev_init(&xor_dev.cdev, &fops);
    xor_dev.cdev.owner = THIS_MODULE;
    
    /* Añadir el dispositivo de caracteres */
    ret = cdev_add(&xor_dev.cdev, dev, 1);
    if (ret) {
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    /* Crear la clase del dispositivo */
    xor_dev.class = class_create(CLASS_NAME);
    if (IS_ERR(xor_dev.class)) {
        ret = PTR_ERR(xor_dev.class);
        cdev_del(&xor_dev.cdev);
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    /* Crear el nodo del dispositivo */
    xor_dev.device = device_create(xor_dev.class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(xor_dev.device)) {
        class_destroy(xor_dev.class);
        cdev_del(&xor_dev.cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(xor_dev.device);
    }

    /* Inicializar mutex y variables */
    mutex_init(&xor_dev.lock);
    xor_dev.data_size = 0;
    xor_dev.xor_key = 'Z';  // Llave XOR por defecto

    printk(KERN_INFO "xor_encrypt: Módulo cargado, número mayor %d\n", major);
    return 0;
}

static void __exit xor_exit(void) {
    dev_t dev = MKDEV(major, 0);
    
    /* Limpiar en orden inverso a la inicialización */
    if (xor_dev.device)
        device_destroy(xor_dev.class, dev);
    if (xor_dev.class)
        class_destroy(xor_dev.class);
    
    cdev_del(&xor_dev.cdev);
    unregister_chrdev_region(dev, 1);
    mutex_destroy(&xor_dev.lock);
    
    printk(KERN_INFO "xor_encrypt: Módulo descargado\n");
}

module_init(xor_init);
module_exit(xor_exit);