#include <cstdlib>
#include <iostream>

#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Channel.hpp"

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string password(argv[2]);
    Server serv(port, password);
    try
    {
        serv.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

//ekrause test
// int main()
// {
//     Client client(10);
//     client.setNickname("ekrause");
//     client.setUsername("NoNoro");
//     Channel channel("test");
//     client.joinChannel(&channel);
//     client.leaveChannel(&channel);

//     std::cout   << client.getNickname() << "\n"
//                 << client.getUsername() << "\n"
//                 << client.getFd()
//                 << std::endl;
// }