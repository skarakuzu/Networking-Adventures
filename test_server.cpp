#include "Server.hpp"
#include<iostream>

std::function<void(int)> shutdown_handler;
void signal_handler(int signal) { shutdown_handler(signal); }

int main()
{
    Server s(AF_INET, SOCK_STREAM, 0, 8089, INADDR_ANY, 10);
    
    std::signal(SIGINT, signal_handler);

    shutdown_handler = [&](int signal) {s.shut_down(); };
    
    s.launch_server();
}
