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
        perror("Failed to open device");
        return 1;
    }

    // Set XOR key
    char key = 'Z';
    if (ioctl(fd, IOCTL_SET_KEY, &key) < 0) {
        perror("Failed to set key");
        close(fd);
        return 1;
    }
    printf("Key set to '%c'\n", key);

    // Write message
    const char *msg = "HelloSecureWorld!";
    write(fd, msg, strlen(msg));
    printf("Encrypted and wrote: %s\n", msg);

    // Reset file offset
    lseek(fd, 0, SEEK_SET);

    // Read message back
    char buf[256] = {0};
    ssize_t bytes = read(fd, buf, sizeof(buf));
    if (bytes < 0) {
        perror("Read failed");
    } else {
        printf("Decrypted and read: %s\n", buf);
    }

    close(fd);
    return 0;
}