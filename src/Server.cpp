#include "../include/Server.hpp"

// orthodox cannonical form

Server::Server() {}

Server::Server(const Server &src)
{
	if (this != &src)
	{
		_server_fd = src._server_fd;
		_port = src._port;
		_password = src._password;
		_clients = src._clients;
		_channels = src._channels;
		_pollfds = src._pollfds;
	}
}

Server Server::operator=(const Server &src)
{
	if (this != &src)
	{
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
Server::Server(int port, std::string &password) : _port(port), _password(password)
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
		throw std::runtime_error("Error: socket failed");

	int opt = 1;
	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);

	if (bind(_server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind failed");
	if (listen(_server_fd, SOMAXCONN) < 0)
		throw std::runtime_error("listen failed");

	fcntl(_server_fd, F_SETFL, O_NONBLOCK);

	std::cout << "Server listening on port " << port << std::endl;
	_pollfds.push_back((pollfd){_server_fd, POLLIN, 0});
}

// runtime
void Server::run()
{
	while (true)
	{
	}
}

void Server::acceptClient()
{
	// need to create a new client here
}

void Server::handleClientMessage(int fd)
{
	// handle message from client + possible commands
}

// handlers
void Server::processCommand(Client &client, const std::string &line)
{
	std::vector < std::string >> tokens = std::split(line);

	std::string cmd = tokens[0];

	if (cmd == "PASS")
		handlePass(client, tokens);
	else if (cmd == "NICK")
		handleNick(client, tokens);
	else if (cmd == "USER")
		handleUser(client, tokens);
	else if (cmd == "JOIN")
		handleJoin(client, tokens);
	else if (cmd == "PRIVMSG")
		handlePrivmsg(client, tokens);
}

void Server::handlePass(Client &client, const std::vector<std::string> &args)
{
	std::cout << "handler called" << std::endl;
}

void Server::handleNick(Client &client, const std::vector<std::string> &args)
{
	std::cout << "handler called" << std::endl;
}

void Server::handleUser(Client &client, const std::vector<std::string> &args)
{
	std::cout << "handler called" << std::endl;
}

void Server::handleJoin(Client &client, const std::vector<std::string> &args)
{
	std::cout << "handler called" << std::endl;
}

void Server::handlePrivmsg(Client &client, const std::vector<std::string> &args)
{
	std::cout << "handler called" << std::endl;
}

void Server::sendToClient(int fd, const std::string &msg)
{
	std::cout << "handler called" << std::endl;
}