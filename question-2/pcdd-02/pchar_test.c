#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd, ret;
    char buf1[32] = "hello";
    char buf2[32] = "";
    // open device file for rd-wr
    fd = open("/dev/pchar0", O_RDWR);
    if (fd < 0)
    {
        perror("failed to open device");
        _exit(1);
    }
    printf("device file opened.\n");
    // write in device file
    ret = write(fd, buf1, strlen(buf1));
    printf("bytes written in device: %d\n", ret);
    // read from device file
    ret = read(fd, buf2, sizeof(buf2));
    printf("bytes read from device: %d -- %s\n", ret, buf2);
    // close device file
    close(fd);
    printf("device file closed.\n");
    return 0;
}






