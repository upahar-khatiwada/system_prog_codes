#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUF_SIZE 1024

int main()
{
    int server_fd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    int client_fds[MAX_CLIENTS] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, MAX_CLIENTS);

    fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);

    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);

    printf("Server listening on port 8080 (non blocking)...\n");

    while (1)
    {
        int new_fd = accept(server_fd, NULL, NULL);
        if (new_fd >= 0)
        {
            fcntl(new_fd, F_SETFL, fcntl(new_fd, F_GETFL, 0) | O_NONBLOCK);

            int added = 0;
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_fds[i] == 0)
                {
                    client_fds[i] = new_fd;
                    printf("New client connected: fd %d\n", new_fd);
                    added = 1;
                    break;
                }
            }
            if (!added)
            {
                printf("Max clients reached, rejecting fd %d\n", new_fd);
                close(new_fd);
            }
        }
        else if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            perror("accept error");
        }

        int n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = '\0';
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if (client_fds[j] > 0)
                    send(client_fds[j], buffer, n, 0);
            }
        }
        else if (n < 0 && errno != EWOULDBLOCK && errno != EAGAIN)
        {
            perror("stdin read error");
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = client_fds[i];
            if (fd <= 0)
                continue;

            int r = recv(fd, buffer, sizeof(buffer) - 1, 0);

            if (r > 0)
            {
                buffer[r] = '\0';
                printf("Client %d: %s", fd, buffer);

                for (int j = 0; j < MAX_CLIENTS; j++)
                {
                    int out_fd = client_fds[j];
                    if (out_fd > 0 && out_fd != fd)
                        send(out_fd, buffer, r, 0);
                }
            }
            else if (r == 0)
            {
                printf("Client fd %d disconnected\n", fd);
                close(fd);
                client_fds[i] = 0;
            }
            else if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                perror("recv error");
                close(fd);
                client_fds[i] = 0;
            }
        }
    }

    close(server_fd);
    return 0;
}