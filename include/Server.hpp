#pragma once

#include <poll.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"

class Client;
class Channel;

class Server {
 public:
  // orthodox cannonical form
  Server();
  Server(const Server& src);
  Server operator=(const Server& src);
  ~Server();

  // other constructor
  Server(int port, std::string& password);

  // runtime
  void run();

 private:
	int _server_fd;
	int _port;
	std::string _password;
	std::map<int, Client*> _clients;
	std::map<std::string, Channel*> _channels;
	std::vector<pollfd> _pollfds;
};
