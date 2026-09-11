#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

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

    printf("Connected. Type messages (they'll be broadcast to other clients):\n");

    while (1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock_fd, &readfds);
        int max_fd = (sock_fd > STDIN_FILENO) ? sock_fd : STDIN_FILENO;

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("select error");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
                break;

            send(sock_fd, buffer, strlen(buffer), 0);
        }

        if (FD_ISSET(sock_fd, &readfds))
        {
            int n = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
            if (n <= 0)
            {
                printf("Server disconnected.\n");
                break;
            }
            buffer[n] = '\0';
            printf("Peer: %s", buffer);
        }
    }

    close(sock_fd);
    return 0;
}