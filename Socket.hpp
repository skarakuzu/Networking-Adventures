#ifndef SOCKET_HPP
#define SOCKET_HPP

#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<iostream>
#include <csignal>

class Socket{
    int socket_fd;
    struct sockaddr_in address;

public:
    Socket() = delete;
    
    // Copy Constructor and copy assignemt not allowed
    Socket(Socket& other) = delete;
    Socket& operator=(Socket& other) = delete;

    // Allow Move constructor and move assignment
    Socket(Socket&& other) noexcept;//  = default;
    Socket& operator=(Socket&& other) noexcept;// = default;

    Socket(int domain, int service, int protocol, u_short port, u_long interface);

    int get_socket_fd();
    struct sockaddr_in get_address();

    int bind_socket();
    int connect_socket();
    int listen_socket(int backlog);
    int accept_connection();
    int close_socket();

    ~Socket();
};

#endif