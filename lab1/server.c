#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    client_fd = accept(server_fd, NULL, NULL);

    while (1)
    {
        int n = recv(client_fd, buffer, sizeof(buffer), 0);

        if (n <= 0)
            break;

        printf("Client: %s", buffer);

        printf("Server: ");
        fgets(buffer, sizeof(buffer), stdin);

        send(client_fd, buffer, strlen(buffer) + 1, 0);
    }

    close(client_fd);
    close(server_fd);

    return 0;
}