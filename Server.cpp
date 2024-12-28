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
    //char buffer[30000] ={0};
    std::string buffer(30000, '\0');

    read(client_socket_fd, buffer.data(), buffer.size()-1);
    std::cout<<"The working thread is with ID: "<< std::this_thread::get_id()<<" with socket ID: "<<client_socket_fd<<std::endl;

    request.parser(buffer);

    std::cout<< buffer <<std::endl;

    char * hello = "Hello from server";
    write(client_socket_fd, hello, strlen(hello));
}

void Server::respond_to_connection()
{
     char * hello = "Hello from server";
     write(client_socket_fd, hello, strlen(hello));
}

void Server::shut_down()
{
    std::cout<<"!!!!!!!!!!! SHUTTING DOWN THE SERVER !!!!!!!!!!!\n";
    interrupted = 1;
    server_socket->close_socket();
    std::terminate;
}


bool Server::get_interrupt_stat()
{
    return interrupted;
}

void Server::launch_server()
{
    Threadpool<std::packaged_task<void()>> tp;

    while(!get_interrupt_stat())
    {
        std::cout<<"=======waiting======\n";

        accept_connection();
        tp.submit(std::packaged_task<void()>([this]{return handle_connection();}));
        //handle_connection();
        //respond_to_connection();
        std::cout<<"=======done=========\n";
    }
}