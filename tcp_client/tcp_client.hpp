#ifndef __TCP_CLIENT_HPP__
#define __TCP_CLIENT_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <cstring>

class tcp_client
{
public:
    tcp_client();
    ~tcp_client();
    int init(const char *host, int port);
    int send(const unsigned char *data, size_t size);
    int recv(unsigned char *data, int size);
    int recv(char* data, int size);
    int deinit();
    int clear_recv_buffer();
private:
    int sockfd;
    struct sockaddr_in server_addr;
};

#endif