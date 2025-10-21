#pragma once

#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <poll.h>

#include "Client.hpp"
#include "Channel.hpp"

class Server
{
    public:
        Server();
        Server(const Server &src);
        Server operator=(const Server &src);
        ~Server();

        Server(int port, std::string &password);
        void run();
    private:
        int _server_fd;
        int _port;
        std::string _password;
        std::map<int, Client*> _clients;
        std::map<std::string, Channel*> channels;
	    std::vector<pollfd> pollfds;

};

