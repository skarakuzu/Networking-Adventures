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
    static thread_local std::string buffer(30000, '\0');

    std::cout<< "AAADDRess of static string buffer: "<<&buffer <<std::endl;

    
    // string.c_str : Returns a pointer a null-terminated C-style string. seems like returning const char * so it cannot be modified
    // string.data:  Does not require nut termination and returns char * 
    read(client_socket_fd, &buffer[0], buffer.size());
    std::cout<<"The working thread is with ID: "<< std::this_thread::get_id()<<" with socket ID: "<<client_socket_fd<<std::endl;
    
    HTTPRequest request;
    
    request.parser(buffer);

    //For chunk testing purposes only , delete later
    /*std::cout<< buffer <<std::endl;
    std::string response_temp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    write(client_socket_fd, response_temp.c_str(), response_temp.size());
    close(client_socket_fd);
    return ;*/

    std::string url = request.get_url();
    std::string filePath = "./public" + url;

    std::cout<<"File path is: "<<filePath<<" "<<filePath.length()<<std::endl;

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
        std::string response = header;
        //std::cout<<"Printing the response: "<<response<<std::endl;
        write(client_socket_fd, response.c_str(), response.size());

        close(fdimg);

        close(client_socket_fd);
        return;
    }
    fstat(fdimg, &stat_buf);
    int img_total_size = stat_buf.st_size;
    int block_size = stat_buf.st_blksize;
    //std::cout<<"total byte vs block bytes vs read buffer is: "<<img_total_size<<" "<<block_size<<std::endl;

    off_t offset = 0;
    off_t len = 0; // Send the entire file
    int flags = 0;


    std::string response = header + request.get_content_type();
    //std::cout<<"Printing the response: "<<response<<std::endl;
    write(client_socket_fd, response.c_str(), response.size());

    int sent_size = 0;
    //std::cout<<"here started sending data with sent_size vs total size: "<<sent_size<<" "<<img_total_size<<std::endl;
    while(img_total_size > 0)
    {
        int send_bytes = ((img_total_size < block_size) ? img_total_size : block_size );
        len = send_bytes;

        // Definition of sendfile on macos is "int sent = sendfile(fd, sockfd, offset, &len, nullptr, flags);". It does file writing with zero copy 
        int done_bytes = sendfile(fdimg, client_socket_fd, offset, &len, NULL, flags); // for MACOS
        //int done_bytes = sendfile(client_socket_fd, fdimg, NULL, send_bytes); / For LINUX

        //std::cout<<"total byte vs block bytes vs read buffer vs len is: "<<img_total_size<<" "<<block_size<<" "<<len<<std::endl;
        offset += len;
        img_total_size = img_total_size - send_bytes;
        sent_size += send_bytes;
    }
    //std::cout<<"here finished sending data with sent_size vs total size: "<<sent_size<<" "<<img_total_size<<std::endl;


    close(fdimg);

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
    //Threadpool<std::packaged_task<void()>> tp;
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