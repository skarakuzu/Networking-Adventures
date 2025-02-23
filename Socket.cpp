#include "Socket.hpp"

Socket::Socket(int domain, int service, int protocol, u_short port,
               u_long interface) {
  memset(&address, 0, sizeof(address));
  address.sin_family = domain;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(interface);

  socket_fd = socket(domain, service, protocol);
  if (socket_fd <= 0)
    throw std::runtime_error("Cannot create the socket");

  std::cout << "Socket Connected with ID: " << socket_fd << std::endl;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  std::cout << "In move assignmet...\n";
  std::exchange(this->socket_fd, other.socket_fd);
  std::exchange(this->address, other.address);

  other.socket_fd = -1;
  other.address.sin_family = 0;
  other.address.sin_port = 0;
  other.address.sin_addr.s_addr = 0;

  return *this;
}

Socket::Socket(Socket &&other) noexcept {
  std::cout << "In move constructor...\n";
  std::exchange(this->socket_fd, other.socket_fd);
  std::exchange(this->address, other.address);

  other.socket_fd = -1;
  other.address.sin_family = 0;
  other.address.sin_port = 0;
  other.address.sin_addr.s_addr = 0;
}

int Socket::get_socket_fd() { return socket_fd; }

struct sockaddr_in Socket::get_address() { return address; }

int Socket::bind_socket() {
  int new_sock = bind(socket_fd, (struct sockaddr *)&address, sizeof(address));
  if (new_sock < 0)
    throw std::runtime_error("Cannot bind to the socket");
  return new_sock;
}

int Socket::accept_connection() {
  socklen_t addrlen = sizeof(address);
  int new_sock = accept(socket_fd, (struct sockaddr *)&address, &addrlen);
  if (new_sock < 0)
    throw std::runtime_error("Cannot accept connections to the socket");
  return new_sock;
}

int Socket::connect_socket() {
  int new_sock =
      connect(socket_fd, (struct sockaddr *)&address, sizeof(address));
  if (new_sock < 0)
    throw std::runtime_error("Cannot connect to the socket");
  return new_sock;
}

int Socket::listen_socket(int backlog) {
  int new_sock = listen(socket_fd, backlog);
  if (new_sock < 0)
    throw std::runtime_error("Cannot listen to the socket");
  return new_sock;
}

int Socket::close_socket() {
  std::cout << "------>>>>>> Closing the socket...\n";
  return close(socket_fd);
}

Socket::~Socket() {
  std::cout << "Calling destructor..\n";
  if (socket_fd > 0) {
    std::cout << "closing socket with fd: " << socket_fd << std::endl;
    close_socket();
  }
}