#ifndef XOR_ENCRYPT_H
#define XOR_ENCRYPT_H

#include <linux/ioctl.h>

/* Nombres de los dispositivos */
#define DEVICE_NAME "xor_encrypt"
#define CLASS_NAME  "xor"

/* Tamaño máximo del búfer */
#define MAX_SIZE    256

/* Comando IOCTL para establecer la clave */
#define IOCTL_SET_KEY _IOW('x', 1, char)

/* Estructura para el dispositivo */
#ifdef __KERNEL__
/* Solo incluir en el kernel */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

/* Estructura para mantener el estado del dispositivo */
struct xor_device {
    struct cdev cdev;
    struct class *class;
    struct device *device;
    char buffer[MAX_SIZE];
    size_t data_size;
    char xor_key;
    struct mutex lock;
};

/* Declaración de funciones del driver */
int xor_open(struct inode *inode, struct file *file);
int xor_release(struct inode *inode, struct file *file);
ssize_t xor_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos);
ssize_t xor_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos);
long xor_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#endif /* __KERNEL__ */

#endif /* XOR_ENCRYPT_H */
