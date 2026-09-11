#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main()
{
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        return 1;
    }

    int flags = fcntl(sock_fd, F_GETFL, 0);
    fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);

    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK);

    printf("Connected (non blocking). Type a message any time.\n");

    while (1)
    {
        int n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = '\0';
            send(sock_fd, buffer, n, 0);
        }
        else if (n < 0 && errno != EWOULDBLOCK && errno != EAGAIN)
        {
            perror("stdin read error");
        }

        int r = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (r > 0)
        {
            buffer[r] = '\0';
            printf("Server: %s", buffer);
        }
        else if (r == 0)
        {
            printf("Server disconnected\n");
            break;
        }
        else if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            perror("recv error");
            break;
        }

        sleep(1);
    }

    close(sock_fd);
    return 0;
}