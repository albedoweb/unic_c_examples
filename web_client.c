// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 80

int open_connection(const char *hostname, int port)
{
    int sd, err;
    struct addrinfo hints = {}, *addrs;
    char port_str[16] = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    sprintf(port_str, "%d", port);

    err = getaddrinfo(hostname, port_str, &hints, &addrs);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", hostname, gai_strerror(err));
        abort();
    }

    for (struct addrinfo *addr = addrs; addr != NULL; addr = addr->ai_next)
    {
        sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sd == -1)
        {
            err = -1;
            break;
        }

        if (connect(sd, addr->ai_addr, addr->ai_addrlen) == 0)
            break;

        err = -1;

        close(sd);
        sd = -1;
    }

    freeaddrinfo(addrs);

    if (sd == -1)
    {
        fprintf(stderr, "%s: %d\n", hostname, err);
        abort();
    }

    return sd;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("\n Usage: ./web_client hostname \n");
        return -1;
    }

    long valread;
    char hello[1024] = {0};
    sprintf(hello, "GET / HTTP/1.1\nHost: %s\nAccept: text/html\nConnection: close\n\n", argv[1]);
    char buffer[1024] = {0};

    int sock = open_connection(argv[1], PORT);
    if (sock <= 0)
    {
        printf("Can't open socket.\n");
        return -1;
    }

    send(sock, hello, strlen(hello), 0);
    printf("\nREQUEST:\n");
    printf("%s\n\n", hello);
    while ((valread = read(sock, buffer, 1023)) > 0)
    {
        buffer[valread] = '\0';
        printf("%s", buffer);
    }
    printf("\n\n");
    return 0;
}
