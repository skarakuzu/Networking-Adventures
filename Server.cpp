#include "Server.hpp"

Server::Server(int domain, int service, int protocol, u_short port, u_long interface, int numconn)
{
    server_socket = std::make_unique<Socket>(domain, service, protocol, port, interface);
    server_socket->bind_socket();
    server_socket->listen_socket(numconn);
}
void Server::accept_connection()
{ 
    client_socket_fd = server_socket->accept_connection();
}

void Server::handle_connection()
{
    read(client_socket_fd, buffer, 30000);
    std::cout<< buffer <<std::endl;
}

void Server::respond_to_connection()
{
     char * hello = "Hello from server";
     write(client_socket_fd, hello, strlen(hello));
}

void Server::launch_server()
{
    while(true)
    {
        std::cout<<"=======waiting======\n";

        accept_connection();
        handle_connection();
        respond_to_connection();
        std::cout<<"=======done=========\n";
    }
}