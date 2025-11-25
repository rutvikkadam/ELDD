#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd, ret;

    // validate cmd line args
    if (argc != 3)
    {
        printf("insufficient cmd line args.\nsyntax: %s </dev/pchar*> \"<data to write>\"\n", argv[0]);
        exit(1);
    }

    // open device file for wr
    fd = open(argv[1], O_WRONLY);
    if (fd < 0)
    {
        perror("failed to open device");
        exit(1);
    }
    printf("device file opened.\n");

    // write to device
    ret = write(fd, argv[2], strlen(argv[2]));
    if (ret < 0)
    {
        perror("write() failed");
        close(fd);
        exit(1);
    }

    printf("Wr - bytes written to device: %d -- %s\n", ret, argv[2]);

    // close device file
    close(fd);
    printf("device file closed.\n");

    return 0;
}

