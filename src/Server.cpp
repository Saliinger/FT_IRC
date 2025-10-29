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

Server::~Server()
{
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
	std::cout << "Server Password: " + _password << std::endl;
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
			if (_pollfds[i].revents & POLLIN)
			{
				if (_pollfds[i].fd == _server_fd) // check for new connection
					acceptClient();
				else // check for new message in fds
					handleClientMessage(_pollfds[i].fd);
			}
		}
	}
}

void Server::acceptClient()
{
	int client_fd = accept(_server_fd, NULL, NULL);
	if (client_fd < 0)
		return;
	_clients[client_fd] = new Client(client_fd);
	_pollfds.push_back((pollfd){client_fd, POLLIN, 0});
	// sendWelcome(*_clients[client_fd]);
	std::string handchake(":ft_irc CAP * LS :\r\n");
	sendToClient(client_fd, handchake);
	std::string handchake2(":ft_irc 461 * JOIN :Not enough parameter\r\n");
	sendToClient(client_fd, handchake2);
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
	char temp[512];
	int bytes_read = recv(fd, temp, sizeof(temp) - 1, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read == 0)
			std::cout << "Client disconnected: " << fd << std::endl;
		else
			std::cerr << "recv() error on fd " << fd << std::endl;
		close(fd);
		_clients.erase(fd);
		_temp_buffer.erase(fd);
		return; // need to remove the fd from the poll list
	}

	temp[bytes_read] = '\0';  // Null-terminate
	_temp_buffer[fd] += temp; // Append to persistent buffer

	size_t pos;
	while ((pos = _temp_buffer[fd].find("\r\n")) != std::string::npos)
	{
		// Extract one full command line
		std::string line = _temp_buffer[fd].substr(0, pos);
		_temp_buffer[fd].erase(0, pos + 2); // Remove processed message

		// Trim leading/trailing whitespace
		if (line.empty())
			continue;

		std::cout << "[FD " << fd << "] cmd: " << line << std::endl;
		Command::handleCommand(*_clients[fd], _channels, line, _password);
	}
}

// when someone join a channel everyone get a message that they joined :alice!alice@localhost JOIN :#chat


// getter
std::string Server::getPass() const {
	return _password;
}