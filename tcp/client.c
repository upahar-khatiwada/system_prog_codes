#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sock;
    char buffer[1024];

    struct sockaddr_in server;

    // 1. Create Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully.\n");

    // 2. Server Address
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0)
    {
        perror("Invalid server address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // 3. Connect
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server successfully.\n");

    // 4. Send Message
    strcpy(buffer, "Hello Server");

    if (send(sock, buffer, strlen(buffer), 0) < 0)
    {
        perror("Send failed");
    }
    else
    {
        printf(" Message sent to server.\n");
    }

    // 5. Receive Reply
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (bytes < 0)
    {
        perror("Receive failed");
    }
    else
    {
        buffer[bytes] = '\0';
        printf("Server says: %s\n", buffer);
    }

    // 6. Close Socket
    close(sock);

    printf("Connection closed.\n");

    return 0;
}