#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "HTTPRequest.hpp"
#include "Socket.hpp"
#include "ThreadPool.hpp"
#include <functional>

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#include "ResponseHandler.hpp"

class Server {

  // int client_socket_fd;
  std::unique_ptr<Socket> server_socket;
  // HTTPRequest request;
  volatile sig_atomic_t interrupted = 0;
  int num_workers;
  int accept_connection();
  void handle_connection(int);
  void respond_to_connection();

public:
  Server(int domain, int service, int protocol, u_short port, u_long interface,
         int numconn);
  void launch_server();
  void shut_down();

  bool get_interrupt_stat();
};
#endif // SERVER_HPP_
