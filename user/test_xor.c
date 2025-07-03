#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "../include/xor_encrypt.h"

#define DEVICE "/dev/" DEVICE_NAME

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Error al abrir el dispositivo");
        return 1;
    }

    // Llave XOR
    char key = 'Z';
    if (ioctl(fd, IOCTL_SET_KEY, &key) < 0) {
        perror("Error al establecer la llave");
        close(fd);
        return 1;
    }
    printf("Llave establecida en '%c'\n", key);

    // Write
    const char *msg = "HolaMundoSeguro!";
    write(fd, msg, strlen(msg));
    printf("Mensaje cifrado y escrito: %s\n", msg);

    // Reiniciar posición del archivo
    lseek(fd, 0, SEEK_SET);

    // Read
    char buf[256] = {0};
    ssize_t bytes = read(fd, buf, sizeof(buf));
    if (bytes < 0) {
        perror("Error al leer");
    } else {
        printf("Mensaje descifrado: %s\n", buf);
    }

    close(fd);
    return 0;
}