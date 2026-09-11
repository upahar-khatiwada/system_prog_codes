#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int server_fd, client_fd;
    char buffer[1024];

    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully\n");

    // server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    // bind
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Bind successful\n");

    // listen
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080\n");

    // accept
    client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected successfully\n");
    printf("Client IP : %s\n", inet_ntoa(client.sin_addr));
    printf("Client Port : %d\n\n", ntohs(client.sin_port));

    int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0)
    {
        perror("Receive failed");
    }
    else
    {
        buffer[bytes] = '\0';
        printf("Client says: %s\n", buffer);
    }

    // 7. Send Response
    strcpy(buffer, "Hello from Server");

    if (send(client_fd, buffer, strlen(buffer), 0) < 0)
        perror("Send failed");
    else
        printf("Reply sent to client.\n");

    // 8. Close
    close(client_fd);
    close(server_fd);

    printf("Connection closed.\n");

    return 0;
}