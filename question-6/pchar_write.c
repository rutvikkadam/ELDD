#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd, ret;

    if (argc != 3)
    {
        printf("syntax: %s </dev/pchar*> \"<data>\"\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_WRONLY);
    if (fd < 0)
    {
        perror("failed to open device");
        exit(1);
    }
    printf("device opened for WRITE. (will block if full)\n");

    ret = write(fd, argv[2], strlen(argv[2]));
    if (ret < 0)
    {
        perror("write() failed");
        close(fd);
        exit(1);
    }

    printf("Wr - bytes written: %d -- %s\n", ret, argv[2]);

    close(fd);
    printf("device closed.\n");
    return 0;
}

