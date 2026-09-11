#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sock;
    char msg[1024];

    struct sockaddr_in serv;
    socklen_t len = sizeof(serv);

    // Create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // Server address
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

    // Send message
    strcpy(msg, "Hello UDP Server");

    sendto(sock,
           msg,
           strlen(msg),
           0,
           (struct sockaddr *)&serv,
           len);

    printf("Message Sent\n");

    // Receive reply
    int n = recvfrom(sock,
                     msg,
                     sizeof(msg) - 1,
                     0,
                     (struct sockaddr *)&serv,
                     &len);

    msg[n] = '\0';

    printf("Server: %s\n", msg);

    // Close
    close(sock);

    return 0;
}