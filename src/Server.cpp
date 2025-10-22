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
	std::memset(&addr, 0, sizeof(addr));
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
		if (poll(&_pollfds[0], _pollfds.size(), -1) < 0)
			throw std::runtime_error("Error: poll failed");
		for (size_t i = 0; i < _pollfds.size(); i++) {
			if (_pollfds[i].events && POLLIN)
				acceptClient();
			else
				handleClientMessage(_pollfds[i].fd);
		}
	}
}

void Server::acceptClient()
{
	// open an fd for the new client
	int client_fd = accept(_server_fd, NULL, NULL);
	if (client_fd < 0) return;
	_clients[client_fd] = new Client(client_fd); // ne pas oublier de delete
	// add client to the pollfds list to check for new messages from
	_pollfds.push_back((pollfd){client_fd, POLLIN, 0});
	std::cout << "Client connected: " << client_fd << std::endl;
}

void Server::handleClientMessage(int fd)
{
	(void)fd;
	std::cout << "message handled" << std::endl;
}

// handlers
void Server::processCommand(Client &client, const std::string &line)
{
	std::vector < std::string > tokens = split(line, ' ');

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
	(void)client;
	(void)args;
	std::cout << "handler called" << std::endl;
}

void Server::handleNick(Client &client, const std::vector<std::string> &args)
{
	(void)client;
	(void)args;
	std::cout << "handler called" << std::endl;
}

void Server::handleUser(Client &client, const std::vector<std::string> &args)
{
	(void)client;
	(void)args;
	std::cout << "handler called" << std::endl;
}

void Server::handleJoin(Client &client, const std::vector<std::string> &args)
{
	(void)client;
	(void)args;
	std::cout << "handler called" << std::endl;
}

void Server::handlePrivmsg(Client &client, const std::vector<std::string> &args)
{
	(void)client;
	(void)args;
	std::cout << "handler called" << std::endl;
}

void Server::sendToClient(int fd, const std::string &msg)
{
	(void)fd;
	(void)msg;
	std::cout << "handler called" << std::endl;
}