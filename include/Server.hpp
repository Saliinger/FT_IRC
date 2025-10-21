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

#include "Channel.hpp"
#include "Client.hpp"

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

	// handlers
	void handlePass(Client &client, const std::vector<std::string> &args);
	void handleNick(Client &client, const std::vector<std::string> &args);
	void handleUser(Client &client, const std::vector<std::string> &args);
	void handleJoin(Client &client, const std::vector<std::string> &args);
	void handlePrivmsg(Client &client, const std::vector<std::string> &args);
	void sendToClient(int fd, const std::string &msg);

private:
	int _server_fd;
	int _port;
	std::string _password;
	std::map<int, Client *> _clients;
	std::map<std::string, Channel *> _channels;
	std::vector<pollfd> _pollfds;
};
