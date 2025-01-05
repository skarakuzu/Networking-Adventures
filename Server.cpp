#include "Server.hpp"

//static thread_local std::string buffer(30000, '\0');

Server::Server(int domain, int service, int protocol, u_short port, u_long interface, int numconn)
{
    num_workers = numconn;
    server_socket = std::make_unique<Socket>(domain, service, protocol, port, interface);
    server_socket->bind_socket();
    server_socket->listen_socket(numconn);
}
int Server::accept_connection()
{ 
    return server_socket->accept_connection();
}

void Server::handle_connection(int client_socket_fd)
{
    std::string buffer(30000, '\0');

    std::cout<< "AAADDRess of static string buffer: "<<&buffer <<" and size: "<<buffer.size()<<std::endl;

    
    // string.c_str : Returns a pointer a null-terminated C-style string. seems like returning const char * so it cannot be modified
    // string.data:  Does not require nut termination and returns char * 
    int readed = read(client_socket_fd, &buffer[0], buffer.size());
    if (readed < 0) { std::cout<<"Cannot read from the socket....\n"; return; }
    //std::cout<< buffer <<std::endl;
    std::cout<<"The working thread is with ID: "<< std::this_thread::get_id()<<" with socket ID: "<<client_socket_fd<<std::endl;
    
    HTTPRequest request;
    
    request.parser(buffer);
    //std::cout<< "Parsed the request: "<<request.get_method()<<std::endl;

    std::shared_ptr<BaseTask> basePtr;
    //std::cout<< "Created the shared pointer: "<<std::endl;

    request.respond(client_socket_fd, std::move(buffer), basePtr);
    
    basePtr->perform();

    close(client_socket_fd);
}

void Server::shut_down()
{
    std::cout<<"!!!!!!!!!!! SHUTTING DOWN THE SERVER !!!!!!!!!!!\n";
    interrupted = 1;
    server_socket->close_socket();
    throw std::runtime_error("Server is shut down!");
}


bool Server::get_interrupt_stat()
{
    return interrupted;
}

void Server::launch_server()
{
    std::cout<<"********** SERVER LAUNCH WELCOME MESSAGE : WAITING FOR TASKS ******************\n";
    //Threadpool<std::packaged_task<void()>> tp(num_workers);
    //Threadpool<std::function<void()>> tp(1);
    Threadpool<std::function<void()>> tp(num_workers);

    while(!get_interrupt_stat())
    {
        std::cout<<"=======waiting======\n";

        int conn = accept_connection();
        //tp.submit(std::packaged_task<void()>([this, conn](){return handle_connection(conn);}));
        tp.submit([this, conn]{return handle_connection(conn);});

        //handle_connection();
        //respond_to_connection();
        std::cout<<"=======done=========\n";
    }
}