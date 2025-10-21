#include "../include/Server.hpp"

// orthodox cannonical form

Server::Server() {}

Server::Server(const Server& src) {
  if (this != &src) {
    _server_fd = src._server_fd;
    _port = src._port;
    _password = src._password;
    _clients = src._clients;
    _channels = src._channels;
    _pollfds = src._pollfds;
  }
}

Server Server::operator=(const Server& src) {
  if (this != &src) {
    _server_fd = src._server_fd;
    _port = src._port;
    _password = src._password;
    _clients = src._clients;
    _channels = src._channels;
    _pollfds = src._pollfds;
  }
  return *this;
}

Server::~Server() {}

// other constructor
Server::Server(int port, std::string& password) {
	_port = port;
	_password = password;
}

// runtime
void Server::run() {
  while (true) {
  }
}