#ifndef REQUESTCONSTS_HPP
#define REQUESTCONSTS_HPP

#include <array>
#include <map>
#include <string_view>

#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

enum {
  HTTP_HEADER,
  FILE_CREATED,
  BAD_REQUEST,
  NOT_FOUND,
  SERVER_ERROR
} messageType;

static constexpr std::array<std::pair<std::string_view, std::string_view>, 12>
    mimetype = {
        std::make_pair("json", "Content-Type: application/json\r\n\r\n"),
        std::make_pair("png", "Content-Type: image/png\r\n\r\n"),
        std::make_pair("txt", "Content-Type: text/plain\r\n\r\n"),
        std::make_pair("html", "Content-Type: text/html\r\n\r\n"),
        std::make_pair("jpg", "Content-Type: image/jpg\r\n\r\n"),
        std::make_pair("php", "Content-Type: application/x-httpd-php\r\n\r\n"),
        std::make_pair("js", "Content-Type: text/javascript\r\n\r\n"),
        std::make_pair("css", "Content-Type: text/css\r\n\r\n"),
        std::make_pair("svg", "Content-Type: image/svg+xml\r\n\r\n"),
        std::make_pair("ico", "Content-Type: image/vnd.microsoft.icon\r\n\r\n"),
        std::make_pair("tiff", "Content-Type: image/tiff\r\n\r\n"),
        std::make_pair("pdf", "Content-Type: application/pdf\r\n\r\n"),
};
static constexpr std::array<std::string_view, 5> responses = {
    "HTTP/1.1 200 OK\r\n", "HTTP/1.1 201 OK\r\n",
    "HTTP/1.1 400 Bad request \r\n", "HTTP/1.1 404 File not found \r\n",
    "HTTP/1.1 500 Internal Server error \r\n"};

#endif