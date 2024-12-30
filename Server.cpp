#include "Server.hpp"

Server::Server(int domain, int service, int protocol, u_short port, u_long interface, int numconn)
{
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
    //int client_socket_fd = accept_connection();
    //char buffer[30000] ={0};
    std::string buffer(30000, '\0');

    read(client_socket_fd, buffer.data(), buffer.size()-1);
    std::cout<<"The working thread is with ID: "<< std::this_thread::get_id()<<" with socket ID: "<<client_socket_fd<<std::endl;
    
    HTTPRequest request;
    
    request.parser(buffer);
    std::string url = request.get_url();
    std::string filePath = "." + url;
    std::cout<<"File path is: "<<filePath<<" "<<filePath.length()<<std::endl;
    //if(filePath.length() <=2)
    //{
    //    filePath = "./index.html"; 
    //}

    //std::cout<< buffer <<std::endl;
    // Completely C now, change to C++ asap
    struct stat stat_buf;
    int fdimg = open(filePath.c_str(), 0);

    std::string header{request.responses[HTTP_HEADER]};
    //std::cout<<"cheking address of request class: "<<&request<<std::endl;
    //std::cout<<"cheking address of static vars: "<<&request.responses<<std::endl;
    //std::cout<<"cheking address of mimetype array: "<<&request.mimetype<<std::endl;
    //std::cout<<"cheking address of messageType enum: "<<&messageType<<std::endl;

    if(fdimg < 0 || filePath.length() <= 2)
    {
        header = std::string(request.responses[NOT_FOUND]);
        printf("cannot open file path: %s,\n", filePath.c_str());
        close(fdimg);

        close(client_socket_fd);
        return;
    }
    fstat(fdimg, &stat_buf);
    int img_total_size = stat_buf.st_size;
    int block_size = stat_buf.st_blksize;
    std::cout<<"total byte vs block bytes is: "<<img_total_size<<" "<<block_size<<std::endl;

    std::string buffer_write(img_total_size, '\0');
    ssize_t bytes_read;

    int read_bytes = read(fdimg, buffer_write.data(), buffer_write.size()-1);
    //std::cout<<"total byte vs sent bytes is: "<<img_total_size<<" "<<read_bytes<<std::endl;

    //std::cout<<"header is: "<<header<<std::endl;

    if (read_bytes > 0)
    {
        //std::string response = header + request.get_content_type() + buffer_write;
        std::string response = header + request.get_content_type();
        //std::cout<<"Printing the response: "<<response<<std::endl;
        write(client_socket_fd, response.c_str(), response.size());

        int send_bytes = ((img_total_size < block_size) ? img_total_size : block_size );
        write(client_socket_fd, buffer_write.c_str(), send_bytes);
        img_total_size = img_total_size - send_bytes;

        size_t sent_size = send_bytes;
        while(img_total_size > 0)
        {
        std::cout<<"here sending data"<<std::endl;
        int send_bytes = ((img_total_size < block_size) ? img_total_size : block_size );
        write(client_socket_fd, buffer_write.c_str()+sent_size, send_bytes);
        img_total_size = img_total_size - send_bytes;
        sent_size += send_bytes;
        }
        std::cout<<"here finished sending data"<<std::endl;

    }

    close(fdimg);

    close(client_socket_fd);
}

void Server::respond_to_connection()
{
     char * hello = "Hello from server";
     //write(client_socket_fd, hello, strlen(hello));
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
    //Threadpool<std::packaged_task<void()>> tp;
    Threadpool<std::function<void()>> tp;


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