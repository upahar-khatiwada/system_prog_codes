#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

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

    printf("Server listening on port 8080...\n");

    while (1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        int max_fd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = client_fds[i];
            if (fd > 0)
                FD_SET(fd, &readfds);
            if (fd > max_fd)
                max_fd = fd;
        }

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("select error");
            continue;
        }

        if (FD_ISSET(server_fd, &readfds))
        {
            int new_fd = accept(server_fd, NULL, NULL);
            if (new_fd >= 0)
            {
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
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            if (fgets(buffer, sizeof(buffer), stdin) != NULL)
            {
                int len = strlen(buffer);
                for (int j = 0; j < MAX_CLIENTS; j++)
                {
                    if (client_fds[j] > 0)
                        send(client_fds[j], buffer, len, 0);
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = client_fds[i];
            if (fd > 0 && FD_ISSET(fd, &readfds))
            {
                int n = recv(fd, buffer, sizeof(buffer) - 1, 0);

                if (n <= 0)
                {
                    printf("Client fd %d disconnected\n", fd);
                    close(fd);
                    client_fds[i] = 0;
                }
                else
                {
                    buffer[n] = '\0';
                    printf("Client %d: %s", fd, buffer);

                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        int out_fd = client_fds[j];
                        if (out_fd > 0 && out_fd != fd)
                            send(out_fd, buffer, n, 0);
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}