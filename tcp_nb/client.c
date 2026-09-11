#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

int main()
{
    int sock;
    char buffer[1024];

    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created.\n");

    // Make socket non-blocking
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    // Non-blocking connect
    while (1)
    {
        int result = connect(sock,
                             (struct sockaddr *)&server,
                             sizeof(server));

        if (result == 0)
        {
            printf("Connected to server.\n");
            break;
        }

        if (errno == EINPROGRESS)
        {
            printf("Connecting...\n");
            sleep(1);
            continue;
        }

        perror("Connect failed");
        close(sock);
        return 1;
    }

    strcpy(buffer, "Hello Server");

    send(sock, buffer, strlen(buffer), 0);

    while (1)
    {
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';
            printf("Server: %s\n", buffer);
            break;
        }
        else if (bytes < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf("Waiting for server reply...\n");
                sleep(1);
            }
            else
            {
                perror("Receive failed");
                break;
            }
        }
    }

    close(sock);

    printf("Client closed.\n");

    return 0;
}