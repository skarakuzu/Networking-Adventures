#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include<iostream>
#include<map>
#include <sstream>
#include<string_view>
#include<array>
#include <algorithm>
#include <fstream>

#include "RequestConsts.h"
#include "ResponseHandler.hpp"

class HTTPRequest
{
    void parse_body(std::string& str, std::map<std::string, std::string>& body_map);

    std::map<std::string, std::string> request_map;
    std::map<std::string, std::string> header_map;
    std::map<std::string, std::string> body_map;
    std::string fileExtension;
    std::string filename_to_write;
    std::string file_boundry;
    std::stringstream ss;

public:
    std::string get_url();
    std::string get_method();

    std::string get_content_type();
    std::string get_fileExtension();
    std::streampos get_buffer_postion();

    void parser(std::string& str);

    void  respond(int socket_id, std::string&& buffer,std::unique_ptr<BaseTask>&);
    void respond_type_get(int socket_id);
    void respond_type_post(int socket_id);
    void respond_type_put(int socket_id);

/*
static constexpr std::array<std::pair<std::string_view, std::string_view> , 10> mimetype = 
{
std::make_pair("json", "Content-Type: application/json\r\n\r\n"),
std::make_pair("png",  "Content-Type: image/png\r\n\r\n"),
std::make_pair("txt",  "Content-Type: text/plain\r\n\r\n"),
std::make_pair("html", "Content-Type: text/html\r\n\r\n"),
std::make_pair("jpg",  "Content-Type: image/jpg\r\n\r\n"),
std::make_pair("php",  "Content-Type: application/x-httpd-php\r\n\r\n"),
std::make_pair("js",   "Content-Type: text/javascript\r\n\r\n"),
std::make_pair("css",  "Content-Type: text/css\r\n\r\n"),
std::make_pair("svg",  "Content-Type: image/svg+xml\r\n\r\n"),
std::make_pair("ico",  "Content-Type: image/vnd.microsoft.icon\r\n\r\n"),
};
static constexpr std::array<std::string_view, 4> responses = 
{
"HTTP/1.1 200 OK\r\n",
"HTTP/1.1 201 OK\r\n",
"HTTP/1.1 400 Bad request \r\n",
"HTTP/1.1 404 File not found \r\n"
};
*/
};

#endif