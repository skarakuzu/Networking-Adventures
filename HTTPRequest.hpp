#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include<iostream>
#include<map>
#include <sstream>
#include<string_view>
#include<array>
#include <algorithm>

enum{
      HTTP_HEADER,
      BAD_REQUEST,
      NOT_FOUND,
 }messageType;

class HTTPRequest
{
    void parse_body(std::string& str, std::map<std::string, std::string>& body_map);

    std::map<std::string, std::string> request_map;
    std::map<std::string, std::string> header_map;
    std::map<std::string, std::string> body_map;
    std::string fileExtension;

public:
    std::string get_url();

    std::string get_content_type();

    void parser(std::string& str);

    static constexpr std::array<std::pair<std::string_view, std::string_view> , 5> mimetype = 
    {
    std::make_pair("json", "application/json\r\n\r\n"),
    std::make_pair("png", "image/png\r\n\r\n"),
    std::make_pair("txt", "text/plain\r\n\r\n"),
    std::make_pair("html", "text/html\r\n\r\n"),
    std::make_pair("jpg", "image/jpg\r\n\r\n")
    };
    static constexpr std::array<std::string_view, 3> responses = 
    {
    "HTTP/1.1 200 OK\r\n",
    "HTTP/1.0 400 Bad request \r\n",
    "HTTP/1.0 404 File not found \r\n"
    };

};


#endif