#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd, ret;
    char buf[64];

    // validate cmd line args
    if (argc != 2)
    {
        printf("insufficient cmd line args.\nsyntax: %s </dev/pchar*>\n", argv[0]);
        exit(1);
    }

    // open device file for rd
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("failed to open device");
        exit(1);
    }
    printf("device file opened.\n");

    // read from device
    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, sizeof(buf)); // this will block if buffer empty

    if (ret < 0)
    {
        perror("read() failed");
        close(fd);
        exit(1);
    }

    printf("Rd - bytes read from device: %d -- %s\n", ret, buf);

    // close device file
    close(fd);
    printf("device file closed.\n");

    return 0;
}

