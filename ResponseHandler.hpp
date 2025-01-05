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
    int content_length;
    std::string buffer;
    std::string filePath;
    std::string content_type;

    virtual ~BaseTask() {} // Virtual destructor for proper cleanup
    virtual int perform() = 0; 

    BaseTask(int socket_id, int priority, int offset, int content_length, std::string&& buffer, std::string fname, std::string content_type) : client_socket_fd(socket_id), priority(priority), offset(offset), content_length(content_length), buffer(std::forward<std::string>(buffer)), filePath(fname), content_type(content_type){}
};

struct WriterTask : public BaseTask{

    WriterTask(int socket_id, int priority, int offset, int content_length, std::string&& buffer, std::string fname, std::string content_type) : BaseTask(socket_id, priority, offset, content_length, std::forward<std::string>(buffer), fname, content_type)
    {
                filePath = "./downloads/" + filePath;
                std::cout<<"*********************** IN WRITER ********: "<<&buffer<<std::endl;
    }

    int perform() override
    {

        std::cout << "Thread ID: " << std::this_thread::get_id() << " writing to the file: "<<filePath<<std::endl;
        std::ofstream outFile(filePath,  std::ios::app); // Create and open a file for writing
        if (outFile.is_open()) 
        {
            outFile.write(buffer.c_str() + offset, content_length); // Write to the file
            outFile.close(); // Close the file

            std::string header{responses[FILE_CREATED]};
            std::string response = header + content_type;
            //std::cout<<"Printing the response: "<<response<<std::endl;
            write(client_socket_fd, response.c_str(), response.size());
            //std::cout << "File written successfully!" << std::endl;
        } else 
        {
            std::string header = std::string(responses[NOT_FOUND]);
            write(client_socket_fd, header.c_str(), header.size());
            //std::cout << "Error opening the file." << std::endl;
        }
        return 1;
    }
};

struct ReaderTask : public BaseTask{

    ReaderTask(int socket_id, int priority, int offset, int content_length, std::string&& buffer, std::string fname, std::string content_type) : BaseTask(socket_id, priority, offset, content_length, std::forward<std::string>(buffer), fname, content_type)
    {
        filePath = "./public" + filePath;
        std::cout<<"*********************** IN READER ********: "<<&buffer<<std::endl;;
    }
    
    int perform() override
    {
        std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
        //std::string filePath = "./public" + filename;

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