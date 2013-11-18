#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define FIFO_NAME "readhistoryFIFO"

int main(int argc, char const *argv[])
{
    char s[300];
    int num, fd;

	mkfifo(FIFO_NAME, 0666);
    printf("waiting for writers...\n");
	fd = open(FIFO_NAME, O_RDONLY);
	printf("got a writer\n");
    do {
        if ((num = read(fd, s, 300)) == -1)
            perror("read");
        else {
            s[num] = '\0';
            printf("%s\n",  s);
        }
    } while (num > 0);
    close(fd);
    unlink(FIFO_NAME);

	return 0;
}