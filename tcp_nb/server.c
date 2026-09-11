#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

int main()
{
    int server_fd, client_fd;
    char buffer[1024];

    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created.\n");

    // Make server socket non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    // Server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client...\n");

    // Non-blocking accept
    while (1)
    {
        client_fd = accept(server_fd,
                           (struct sockaddr *)&client,
                           &client_len);

        if (client_fd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf("No client yet...\n");
                sleep(1);
                continue;
            }
            else
            {
                perror("Accept failed");
                close(server_fd);
                exit(EXIT_FAILURE);
            }
        }

        printf("Client connected!\n");
        break;
    }

    flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

    while (1)
    {
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';
            printf("Client: %s\n", buffer);

            strcpy(buffer, "Hello from Non-Blocking Server");
            send(client_fd, buffer, strlen(buffer), 0);

            break;
        }
        else if (bytes < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf("Waiting for client message...\n");
                sleep(1);
            }
            else
            {
                perror("Receive failed");
                break;
            }
        }
    }

    close(client_fd);
    close(server_fd);

    printf("Server closed.\n");

    return 0;
}