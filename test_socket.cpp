#include<iostream>
#include "Socket.hpp"

int main()
{
    std::cout<<"......Testing move assignemt.....\n";
    Socket s(AF_INET, SOCK_STREAM, 0, 8087, INADDR_ANY);
    
    std::cout<<"sock s before move : "<< s.get_socket_fd()<<std::endl;
    std::cout<<s.get_address().sin_addr.s_addr<<std::endl;
    std::cout<<s.get_address().sin_port<<std::endl;
    std::cout<<s.get_address().sin_family<<std::endl;
   
    Socket s1;
    s1 = std::move(s);

    std::cout<<"new sock s1: "<< s1.get_socket_fd()<<std::endl;
    std::cout<<s1.get_address().sin_addr.s_addr<<std::endl;
    std::cout<<s1.get_address().sin_port<<std::endl;
    std::cout<<s1.get_address().sin_family<<std::endl;

    std::cout<<"sock s after move : "<< s.get_socket_fd()<<std::endl;
    std::cout<<s.get_address().sin_addr.s_addr<<std::endl;
    std::cout<<s.get_address().sin_port<<std::endl;
    std::cout<<s.get_address().sin_family<<std::endl;

    std::cout<<"......Testing move constructor.....\n";

    Socket s2(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);

    std::cout<<"sock s2 before move : "<< s2.get_socket_fd()<<std::endl;
    std::cout<<s2.get_address().sin_addr.s_addr<<std::endl;
    std::cout<<s2.get_address().sin_port<<std::endl;
    std::cout<<s2.get_address().sin_family<<std::endl;

    Socket s3(std::move(s2));

    std::cout<<"new sock s3: "<< s3.get_socket_fd()<<std::endl;
    std::cout<<s3.get_address().sin_addr.s_addr<<std::endl;
    std::cout<<s3.get_address().sin_port<<std::endl;
    std::cout<<s3.get_address().sin_family<<std::endl;

    std::cout<<"sock s2 after move : "<< s2.get_socket_fd()<<std::endl;
    std::cout<<s2.get_address().sin_addr.s_addr<<std::endl;
    std::cout<<s2.get_address().sin_port<<std::endl;
    std::cout<<s2.get_address().sin_family<<std::endl;
}
