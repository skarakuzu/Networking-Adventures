#include "Server.hpp"
#include<iostream>

int main()
{
    Server s(AF_INET, SOCK_STREAM, 0, 8089, INADDR_ANY, 10);
    s.launch_server();
}
