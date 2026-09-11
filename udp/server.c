#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sfd;
    char msg[1024];

    struct sockaddr_in serv, cli;
    socklen_t len = sizeof(cli);

    // Create socket
    sfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Server address
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8080);
    serv.sin_addr.s_addr = INADDR_ANY;

    // Bind
    bind(sfd, (struct sockaddr *)&serv, sizeof(serv));

    printf("UDP Server is waiting...\n");

    // Receive message
    int n = recvfrom(sfd,
                     msg,
                     sizeof(msg) - 1,
                     0,
                     (struct sockaddr *)&cli,
                     &len);

    msg[n] = '\0';

    printf("Client Connected\n");
    printf("IP   : %s\n", inet_ntoa(cli.sin_addr));
    printf("Port : %d\n", ntohs(cli.sin_port));
    printf("Client: %s\n", msg);

    // Send reply
    strcpy(msg, "Hello from UDP Server");

    sendto(sfd,
           msg,
           strlen(msg),
           0,
           (struct sockaddr *)&cli,
           len);

    // Close
    close(sfd);

    return 0;
}