#include "HTTPRequest.hpp"

void HTTPRequest::parse_body(std::string& line, std::map<std::string, std::string>& body_map)
{    
     int pos, ref_pos, end_pos, dummy;
     std::string str, first, second; 

        ref_pos=0;
        while(line.find('=', ref_pos) != std::string::npos)
        {
            pos = line.find('=', ref_pos);
            if (pos != std::string::npos)
            {
            dummy = line.find('&', ref_pos);
            end_pos = (dummy == std::string::npos ? line.length() : dummy);
            first= line.substr(ref_pos, pos-ref_pos);
            second = line.substr(pos+1, end_pos-pos-1);
            body_map.insert({first, second});
            ref_pos = end_pos +1;
            }
            else 
                break;

        }
}

void HTTPRequest::parser(std::string& request_str)
{
    //std::map<std::string, std::string> request_map;
    //std::map<std::string, std::string> header_map;
    //std::map<std::string, std::string> body_map;

    //std::stringstream ss(request_str);
    ss.str(request_str);

    int pos, pos_dot, end_pos, dummy;
    std::string str, line, first, second, version, method, url; 

    ss >> method >> line >> version;
    request_map.insert({"method", method});
    request_map.insert({"version", version});

    if (line.length() <= 2 && method == "GET") line = "/index.html";

    pos = line.find_first_of('?');
    pos_dot = line.find_first_of('.');
    if(pos != std::string::npos)
    {
        url = line.substr(0, pos);
        str = line.substr(pos+1);
        parse_body(str, body_map);

        if (method == "GET") fileExtension = pos_dot != std::string::npos ? url.substr(pos_dot+1) : "html";
        if (pos_dot != std::string::npos) request_map.insert({"url", url});
        else request_map.insert({"url", url + "/index." + fileExtension});
    }
    else
    {
        if (method == "GET") fileExtension = pos_dot != std::string::npos ? line.substr(pos_dot+1) : "html";
        if (pos_dot != std::string::npos) request_map.insert({"url", line});
        else request_map.insert({"url", line + "/index." + fileExtension});
    }
    
    //std::cout<<"The file extension is: "<<fileExtension<<std::endl;

    //std::cout<<"reading ...: "<<method<<" "<<url<<" "<<version<<std::endl;
    //std::cout<<"********* started printing *******************\n";
    std::getline(ss, line); //get rid of the request line
    while (std::getline(ss, line)) 
    {

        //if(line.length() == 1) break;
        if(line.length() <= 1) break;
        //std::cout<<"Here with the line: "<<line<<std::endl;
        pos = line.find_first_of(':');
        if (pos != std::string::npos)
        {
        first= line.substr(0, pos);
        second = line.substr(pos+2);

        header_map.insert({first, second});
        }

    }

    if (header_map["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos) 
    {
        // expecting data in the form MyVariableOne=ValueOne&MyVariableTwo=ValueTwo
        while (std::getline(ss, line)) 
        {
            //std::cout<<"Continue Here with the line: "<<line<<std::endl;
            parse_body(line, body_map);
        }
    }
    else if(header_map["Content-Type"].find("multipart/form-data") != std::string::npos)
    {
        std::getline(ss, line); 
        file_boundry = line.substr(2);
        std::cout<<"boundry "<<file_boundry<<std::endl;

        std::getline(ss, line);
        std::cout<<"the line "<<line<<std::endl;
        size_t pos = line.find("filename="); // "filename=" has 10 characters and start reading from there
        filename_to_write = line.substr(pos+10, line.size()-2-pos-10); // get rid of " beginning and end of the filename

        std::cout<<"the filename : "<<filename_to_write<<std::endl;

        std::getline(ss, line); //get rid of the empty line
        std::streampos index = ss.tellg();
        std::cout<<"the request string start: "<<request_str[index]<<std::endl;
    }

    /*
    std::cout<<"printing request map..\n";
    for (const auto& pair : request_map)
        std::cout << pair.first << " " << pair.second << std::endl;
    */
    /*
    std::cout<<"printing header map..\n";
    for (const auto& pair : header_map)
        std::cout << pair.first << " " << pair.second << std::endl;
    */
    /*
    std::cout<<"printing body map..\n";
    for (const auto& pair : body_map)
        std::cout << pair.first << " " << pair.second << std::endl;
    std::cout<<"*********** ended printing ****************\n";
    */
}

/*
void HTTPRequest::respond_type_get(int client_socket_fd)
{
    std::string filePath = "./public" + get_url();

    std::cout<<"File path is: "<<filePath<<" "<<filePath.length()<<std::endl;


    struct stat stat_buf;
    int fdimg = open(filePath.c_str(), 0);

    std::string header{responses[HTTP_HEADER]};
    //std::cout<<"cheking address of request class: "<<&request<<std::endl;
    //std::cout<<"cheking address of static vars: "<<&request.responses<<std::endl;
    //std::cout<<"cheking address of mimetype array: "<<&request.mimetype<<std::endl;
    //std::cout<<"cheking address of messageType enum: "<<&messageType<<std::endl;

    if(fdimg < 0 || filePath.length() <= 2)
    {
        header = std::string(responses[NOT_FOUND]);
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


    std::string response = header + get_content_type();
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
        if (done_bytes < 0) { std::cout<<"Cannot read from the socket....\n"; return; }

        //std::cout<<"total byte vs block bytes vs read buffer vs len is: "<<img_total_size<<" "<<block_size<<" "<<len<<std::endl;
        offset += len;
        img_total_size = img_total_size - send_bytes;
        sent_size += send_bytes;
    }
    //std::cout<<"here finished sending data with sent_size vs total size: "<<sent_size<<" "<<img_total_size<<std::endl;


    close(fdimg);

}
*/
void HTTPRequest::respond_type_post(int client_socket_fd)
{

}
void HTTPRequest::respond_type_put(int client_socket_fd)
{
}

void HTTPRequest::respond(int socket_id, std::string&& buffer, std::unique_ptr<BaseTask>& basePtr )
{        
    std::cout<<"In respond function: "<<request_map["method"]<<std::endl;
    if(request_map["method"] == "GET")
    {    
        std::string content_type = get_content_type();
        //respond_type_get(socket_id);
        //std::cout<<"In GET method \n";
        basePtr = std::make_unique<ReaderTask>(ReaderTask(socket_id, 0, 0, 0, std::forward<std::string>(buffer), get_url(), content_type));
        
    }
    else if(request_map["method"] == "POST")
    {
        std::cout<<"In POST method \n";
        //respond_type_post(socket_id);
        std::string content_type = get_content_type();
        //basePtr = std::make_unique<WriterTask>(WriterTask(socket_id, 0, 0, std::forward<std::string>(buffer), get_url(), content_type));
    }
    else if(request_map["method"] == "PUT")
    {        
        std::cout<<"In PUT method \n";
        //respond_type_put(socket_id);
        basePtr = std::make_unique<WriterTask>(WriterTask(socket_id, 0, get_buffer_postion(), std::stoi(header_map["Content-Length"]), std::forward<std::string>(buffer), filename_to_write, " "));
    }
        std::cout<<"Ended respond function: "<<request_map["method"]<<std::endl;


}

std::streampos HTTPRequest::get_buffer_postion()
{
    return ss.tellg();
}

std::string HTTPRequest::get_url()
{
    return request_map["url"];
}
std::string HTTPRequest::get_method()
{
    return request_map["method"];
}

std::string HTTPRequest::get_fileExtension()
{
    return fileExtension;
}


std::string HTTPRequest::get_content_type()
{
       auto it = std::find_if(mimetype.begin(), mimetype.end(), [this](const auto& p) { return p.first == fileExtension; });
       return it != mimetype.end() ? std::string(it->second) : nullptr;
}
