#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "include/basic_kernel_driver.h"

int main() {
    int fd = open("/dev/memdev", O_RDWR);
    if (fd < 0) {
        printf("Error opening device\n");
        return -1;
    }

    // Write to the device
    char write_buf[] = "Hello, kernel!";
    write(fd, write_buf, strlen(write_buf));

    // Read from the device
    char read_buf[1024] = {0};
    read(fd, read_buf, sizeof(read_buf));
    printf("Read from device: %s\n", read_buf);

    // Resize the buffer via IOCTL
    int new_size = 2048;
    ioctl(fd, IOCTL_SET_BUFFER_SIZE, &new_size);

    // Close the device
    close(fd);
    return 0;
}
