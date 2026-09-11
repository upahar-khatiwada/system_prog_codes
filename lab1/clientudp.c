#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    while (1)
    {
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);

        sendto(sockfd, buffer, strlen(buffer) + 1, 0,
               (struct sockaddr *)&server_addr, len);

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                 NULL, NULL);

        printf("Server: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    close(sockfd);
    return 0;
}