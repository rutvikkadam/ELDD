#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd;

    if (argc != 2)
    {
        printf("syntax: %s </dev/pchar*>\n", argv[0]);
        exit(1);
    }

    printf("Attempting to open device... (will block if already open)\n");

    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("failed to open device");
        exit(1);
    }

    printf("Device opened successfully.\n");
    printf("Press ENTER to close and allow next opener...\n");
    getchar();

    close(fd);
    printf("device closed.\n");
    return 0;
}

