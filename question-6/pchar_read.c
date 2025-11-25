#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd, ret;
    char buf[64];

    if (argc != 2)
    {
        printf("syntax: %s </dev/pchar*>\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("failed to open device");
        exit(1);
    }
    printf("device opened for READ. (will block if empty)\n");

    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, sizeof(buf));
    if (ret < 0)
    {
        perror("read() failed");
        close(fd);
        exit(1);
    }

    printf("Rd - bytes read: %d -- %s\n", ret, buf);

    close(fd);
    printf("device closed.\n");
    return 0;
}

