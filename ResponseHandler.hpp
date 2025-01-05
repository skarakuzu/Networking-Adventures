#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "RequestConsts.h"

struct BaseTask {
    int client_socket_fd;
    int priority;
    int offset;
    std::string buffer;
    std::string filename;
    std::string content_type;

    virtual ~BaseTask() {} // Virtual destructor for proper cleanup
    virtual int perform() = 0; 

    BaseTask(int socket_id, int priority, int offset, std::string buffer, std::string filename, std::string content_type) : client_socket_fd(socket_id), priority(priority), offset(offset), buffer(std::forward<std::string>(buffer)), filename(filename), content_type(content_type)
    {
    }
};

struct WriterTask : BaseTask{

    WriterTask(int socket_id, int priority, int offset, std::string buffer, std::string filename, std::string content_type) : BaseTask(socket_id, priority, offset, std::forward<std::string>(buffer), filename, content_type)
    {
                std::cout<<"*********************** IN WRITER ********: "<<&buffer<<std::endl;
    }

    int perform() override
    {
        std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
        std::ofstream myFile(filename,  std::ios::app); // Create and open a file for writing
        if (myFile.is_open()) {
            myFile << buffer << std::endl; // Write to the file
            myFile.close(); // Close the file
            //std::cout << "File written successfully!" << std::endl;
        } else {
            //std::cout << "Error opening the file." << std::endl;
        }
        return 1;
    }
};

struct ReaderTask : BaseTask{

    ReaderTask(int socket_id, int priority, int offset, std::string buffer, std::string filename, std::string content_type) : BaseTask(socket_id, priority, offset, std::forward<std::string>(buffer), filename, content_type)
    {
        std::cout<<"*********************** IN READER ********: "<<&buffer<<std::endl;;
    }
    
    int perform() override
    {
        std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
        std::string filePath = "./public" + filename;

        std::cout<<"File path is: "<<filePath<<" "<<filePath.length()<<std::endl;


        struct stat stat_buf;
        int fdimg = open(filePath.c_str(), 0);
        std::cout<<"before cheking the header: "<<fdimg<<std::endl;

        std::string header{responses[HTTP_HEADER]};
        //std::cout<<"cheking address of request class: "<<&request<<std::endl;
        //std::cout<<"cheking address of static vars: "<<&request.responses<<std::endl;
        //std::cout<<"cheking address of mimetype array: "<<&request.mimetype<<std::endl;
        //std::cout<<"cheking address of messageType enum: "<<&messageType<<std::endl;
        std::cout<<"cheking the header: "<<header<<std::endl;

    if(fdimg < 0 || filePath.length() <= 2)
    {
        header = std::string(responses[NOT_FOUND]);
        printf("cannot open file path: %s,\n", filePath.c_str());
        std::string response = header;
        //std::cout<<"Printing the response: "<<response<<std::endl;
        write(client_socket_fd, response.c_str(), response.size());

        close(fdimg);

        close(client_socket_fd);
        return 0;
    }
    fstat(fdimg, &stat_buf);
    int img_total_size = stat_buf.st_size;
    int block_size = stat_buf.st_blksize;
    //std::cout<<"total byte vs block bytes vs read buffer is: "<<img_total_size<<" "<<block_size<<std::endl;

    off_t offset = 0;
    off_t len = 0; // Send the entire file
    int flags = 0;


    std::string response = header + content_type;
    //std::cout<<"Printing the response: "<<response<<std::endl;
    write(client_socket_fd, response.c_str(), response.size());

    int sent_size = 0, done_bytes, send_bytes;
    //std::cout<<"here started sending data with sent_size vs total size: "<<sent_size<<" "<<img_total_size<<std::endl;
    while(img_total_size > 0)
    {
        send_bytes = ((img_total_size < block_size) ? img_total_size : block_size );
        len = send_bytes;

        // Definition of sendfile on macos is "int sent = sendfile(fd, sockfd, offset, &len, nullptr, flags);". It does file writing with zero copy 
        done_bytes = sendfile(fdimg, client_socket_fd, offset, &len, NULL, flags); // for MACOS
        //done_bytes = sendfile(client_socket_fd, fdimg, NULL, send_bytes); / For LINUX
        if (done_bytes < 0) { std::cout<<"Cannot read from the socket....\n"; return 0; }

        //std::cout<<"total byte vs block bytes vs read buffer vs len is: "<<img_total_size<<" "<<block_size<<" "<<len<<std::endl;
        offset += len;
        img_total_size = img_total_size - send_bytes;
        sent_size += send_bytes;
    }
    //std::cout<<"here finished sending data with sent_size vs total size: "<<sent_size<<" "<<img_total_size<<std::endl;


    close(fdimg);

        return 1;
    }
};

#endif