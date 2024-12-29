#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include<iostream>
#include<map>
#include <sstream>
#include<string_view>

/*
GET /index.html HTTP/1.1
Host: 127.0.0.1:8089
Sec-Fetch-Dest: document
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.1.1 Safari/605.1.15
Upgrade-Insecure-Requests: 1
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Accept-Language: en-US,en;q=0.9
Priority: u=0, i
Accept-Encoding: gzip, deflate
Connection: keep-alive
 */


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

public:
    std::string get_url();
    void parser(std::string& str);
    
    static const std::array<std::string, 3> responses;

};


#endif