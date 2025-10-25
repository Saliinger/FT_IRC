#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <exception>
#include <cstring>

#include "Channel.hpp"
#include "Client.hpp"
#include "Utils.hpp"
#include "Command.hpp"

class Client;
class Channel;

class Server
{
public:
	// orthodox cannonical form
	Server();
	Server(const Server &src);
	Server operator=(const Server &src);
	~Server();

	// other constructor
	Server(int port, std::string &password);

	// runtime
	void run();
	void acceptClient();
	void handleClientMessage(int fd);
	void sendWelcome(Client &client);
	void sendToClient(int fd, const std::string &msg);

private:
	int _server_fd;								// fd of the serv for the calls to connect and send messages
	int _port;									// port to register and listen from
	std::string _password;						// passwd set at initialisation
	std::map<int, Client *> _clients;			// client accepted on the serv
	std::map<std::string, Channel *> _channels; // channels hosted by the serv
	std::vector<pollfd> _pollfds;				// fds of all client to listen if there's any thing sent from
};
