#include "Socket.hpp"

Socket::Socket(int domain, int service, int protocol, u_short port, u_long interface)
{
    memset(&address, 0, sizeof(address));
    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);

    socket_fd = socket(domain, service, protocol);
}

Socket& Socket::operator=(Socket&& other) noexcept
{
    std::cout<<"In move assignmet...\n";
    std::exchange(this->socket_fd, other.socket_fd);
    std::exchange(this->address, other.address);
    std::exchange(this->connection, other.connection);

    other.socket_fd = -1;
    other.address.sin_family = 0;
    other.address.sin_port = 0;
    other.address.sin_addr.s_addr = 0;
    other.connection = -1;

    return *this;
}

Socket::Socket(Socket&& other) noexcept
{
    std::cout<<"In move constructor...\n";
    std::exchange(this->socket_fd, other.socket_fd);
    std::exchange(this->address, other.address);
    std::exchange(this->connection, other.connection);

    other.socket_fd = -1;
    other.address.sin_family = 0;
    other.address.sin_port = 0;
    other.address.sin_addr.s_addr = 0;
    other.connection = -1;
}

int Socket::get_socket_fd()
{
    return socket_fd;
}
int Socket::get_connection()
{
    return connection;
}

struct sockaddr_in Socket::get_address()
{
    return address;
}

int Socket::bind_socket()
{
    return bind(socket_fd, (struct sockaddr* )& address, sizeof(address));
}

int Socket::accept_connection()
{
    socklen_t addrlen = sizeof(address);
    return accept(socket_fd, (struct sockaddr *)&address, &addrlen);
}

int Socket::connect_socket()
{
    return connect(socket_fd, (struct sockaddr *)&address, sizeof(address));
}

int Socket::listen_socket(int backlog)
{
    return listen(socket_fd, backlog);
}

int Socket::close_socket()
{
    return close(socket_fd);
}

Socket::~Socket()
{
    std::cout<<"Calling destructor..\n";
    if (socket_fd > 0)
    {
        std::cout<<"closing socket with fd: "<<socket_fd<<std::endl;
        close_socket();
    }
}