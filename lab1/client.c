#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    while (1)
    {
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);

        send(sock_fd, buffer, strlen(buffer) + 1, 0);

        int n = recv(sock_fd, buffer, sizeof(buffer), 0);

        if (n <= 0)
            break;

        printf("Server: %s", buffer);
    }

    close(sock_fd);

    return 0;
}