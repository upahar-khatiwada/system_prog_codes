#include <sys/select.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    FD_SET(1, &rfds);
    FD_SET(2, &rfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv);

    if (retval == -1)
    {
        perror("select()");
    }
    else if (retval)
    {
        printf("Data is available now\n");
    }
    else
    {
        printf("No data within 5 seconds\n");
    }

    return 0;
}
