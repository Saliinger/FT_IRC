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

Server::~Server() {
	// delete client / channel
	// close all fd ( client / server )
}

// other constructor
Server::Server(int port, std::string &password) : _port(port), _password(password)
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0); // open a socket on the ipv4
	if (_server_fd < 0)
		throw std::runtime_error("Error: socket failed");

	int opt = 1;
	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // set the socket setting : tell that the port is reserved for that program in case of restart

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;		   // setup for ipv4
	addr.sin_addr.s_addr = INADDR_ANY; // idk
	addr.sin_port = htons(_port);	   // set the port of hour soket

	if (bind(_server_fd, (sockaddr *)&addr, sizeof(addr)) < 0) // attach socket to ip && port
		throw std::runtime_error("bind failed");
	if (listen(_server_fd, SOMAXCONN) < 0) // tell that the port is ready to liste
		throw std::runtime_error("listen failed");

	fcntl(_server_fd, F_SETFL, O_NONBLOCK); // set the fd to non-blocking

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
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].fd == _server_fd) // check for new connection
				acceptClient();
			else if (_pollfds[i].events && POLLIN) // check for new message in fds
				handleClientMessage(_pollfds[i].fd);
		}
	}
}

void Server::sendWelcome(Client &client)
{
	std::string nick = client.getNickname();
	sendToClient(client.getFd(), ":ft_irc 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!\r\n");
	sendToClient(client.getFd(), ":ft_irc 002 " + nick + " :Your host is ft_irc, running version 1.0\r\n");
	sendToClient(client.getFd(), ":ft_irc 003 " + nick + " :This server was created today\r\n");
	sendToClient(client.getFd(), ":ft_irc 004 " + nick + " ft_irc 1.0 o o\r\n");
}

void Server::acceptClient()
{
	int client_fd = accept(_server_fd, NULL, NULL);
	if (client_fd < 0)
		return;
	_clients[client_fd] = new Client(client_fd);
	_pollfds.push_back((pollfd){client_fd, POLLIN, 0});
	sendWelcome(*_clients[client_fd]);
}

// max buffer[512]
// recv to receive
// check for /r/n for end message
// if exceed need to send an error
// if recv(fd, buffer,sizeof(buffer), -1, 0) == 0 client disconnected
// if recv(fd, buffer,sizeof(buffer), -1, 0) == -1 recv error
// if recv(fd, buffer,sizeof(buffer), -1, 0) > 0 text received

void Server::handleClientMessage(int fd)
{
	// put the message in the buuffer if complete send it to handle command and remove the handled message
	char temp_buffer[512]; // temp buffer to test stuff
	int bytes_read = recv(fd, temp_buffer, sizeof(temp_buffer), 0);

	if (bytes_read < 0)
		std::cout << "Error: message failed" << std::endl;

	std::string cmd = temp_buffer;
	// std::cout << "msg: " + cmd << std::endl;
	Command::handleCommand(*_clients[fd], _channels, cmd);

	// clear the buffer for test
	for (size_t i = 0; i < 512; i++)
		temp_buffer[i] = 0;
	// end test
}

// when someone join a channel everyone get a message that they joined :alice!alice@localhost JOIN :#chat