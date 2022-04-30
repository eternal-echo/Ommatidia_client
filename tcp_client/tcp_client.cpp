#include "tcp_client.hpp"

tcp_client::tcp_client()
{
    sockfd = -1;
}

tcp_client::~tcp_client()
{
    deinit();
}

int tcp_client::init(const char *host, int port)
{
    struct hostent *server;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        std::cout << "Failed to create socket" << std::endl;
        return -1;
    }

    server = gethostbyname(host);
    if(server == NULL) {
        std::cout<< "Failed to get host" << std::endl;
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout<< "Failed to connect to server" << std::endl;
        ::close(sockfd);
        return -1;
    }

    return 0;
}

int tcp_client::send(const unsigned char *data, size_t size)
{
    int ret;

    ret = ::send(sockfd, data, size, 0);
    if(ret < 0) {
        std::cout<< "Failed to send data" << std::endl;
        return -1;
    }

    return 0;
}

int tcp_client::recv(unsigned char *data, int size)
{
    int ret;

    ret = ::recv(sockfd, data, size, 0);
    if(ret < 0) {
        std::cout<< "Failed to recv data" << std::endl;
    }

    return ret;
}

int tcp_client::recv(char* data, int size)
{
    int ret;

    ret = ::recv(sockfd, data, size, 0);
    if(ret < 0) {
        std::cout<< "Failed to recv data" << std::endl;
    }

    return ret;
}

int tcp_client::deinit()
{
    ::close(sockfd);
    sockfd = -1;

    return 0;
}

int tcp_client::clear_recv_buffer()
{
    char buf[1024];
    int ret;
    do {
        ret = ::recv(sockfd, buf, sizeof(buf), MSG_DONTWAIT);
    } while(ret > 0);
    return 0;
}