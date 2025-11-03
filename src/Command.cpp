#include "../include/Command.hpp"

Command::Command()
{
	createCommandMap();
}

Command::Command(const Command &src)
{
	(void)src;
	*this = src;
}

Command &Command::operator=(const Command &src)
{
	(void)src;
	return *this;
}

Command::~Command() {}

void Command::createCommandMap()
{
	// Unauthenticated commands - only registration commands allowed
	_unauthCommand["NICK"] = &Command::handleNick;
	_unauthCommand["USER"] = &Command::handleUser;
	// Note: PASS is handled separately because it needs the password parameter

	// Authenticated commands - channel operations
	_authCommand["JOIN"] = &Command::handleJoin;
	_authCommand["PRIVMSG"] = &Command::handlePrivmsg;
	_authCommand["QUIT"] = &Command::handleQuit;
	_authCommand["LEAVE"] = &Command::handleLeave;
	_authCommand["MODE"] = &Command::handleMode;
	_authCommand["TOPIC"] = &Command::handleTopic;
	_authCommand["INVITE"] = &Command::handleInvite;
	// Note: NICK and USER handled specially in handleAuthenticatedCommand
}

void Command::run(Client &client, std::map<std::string, Channel *> &channels, std::string &command, const std::string pass)
{
	std::vector<std::string> tokens = split(command, ' ');
	std::string cmd = tokens[0];

	// PING always responds
	if (cmd == "PING")
	{
		sendToClient(client.getFd(), "PONG :ft_irc\r\n");
		return;
	}

	if (!client.isAuthenticated())
	{
		handleUnauthenticatedCommand(client, cmd, tokens, pass);
		checkAndCompleteRegistration(client, channels, command, pass);
	}
	else
	{
		handleAuthenticatedCommand(client, channels, cmd, tokens);
	}
}

void Command::handleUnauthenticatedCommand(Client &client, const std::string &cmd, const std::vector<std::string> &tokens, const std::string &pass)
{
	// Special case for PASS command (needs password parameter)
	if (cmd == "PASS")
	{
		handlePass(client, tokens, pass);
		return;
	}

	// Look up other unauthenticated commands
	std::map<std::string, UnauthHandler>::iterator it = _unauthCommand.find(cmd);
	if (it == _unauthCommand.end())
		return; // Command not found - silently ignore

	// Call member function pointer - syntax: (this->*functionPointer)(args)
	(this->*(it->second))(client, tokens);
}

void Command::handleAuthenticatedCommand(Client &client, std::map<std::string, Channel *> &channels, std::string &cmd, std::vector<std::string> &tokens)
{
	// Special cases for commands that don't need channel map
	if (cmd == "NICK")
	{
		handleNick(client, tokens);
		return;
	}
	if (cmd == "USER")
	{
		sendToClient(client.getFd(), ":ft_irc 462 :You may not reregister\r\n");
		return;
	}
	if (cmd == "PASS")
	{
		sendToClient(client.getFd(), ":ft_irc 462 :You may not reregister\r\n");
		return;
	}

	// Look up authenticated command
	std::map<std::string, AuthHandler>::iterator it = _authCommand.find(cmd);
	if (it == _authCommand.end())
	{
		sendToClient(client.getFd(), ":ft_irc 421 " + cmd + " :Unknown command\r\n");
		return;
	}

	// Call member function pointer - syntax: (this->*functionPointer)(args)
	(this->*(it->second))(client, channels, tokens);
}

void Command::checkAndCompleteRegistration(
	Client &client,
	std::map<std::string, Channel *> &channels,
	std::string &command,
	const std::string &pass)
{
	if (!client.getNickname().empty() && !client.getUsername().empty())
	{
		client.authenticate();
		sendWelcomeMessage(client);
		run(client, channels, command, pass);
	}
}

void Command::handlePass(Client &client, const std::vector<std::string> &args, const std::string pass)
{
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_PASSWDMISMATCH, client.getNickname(), "PASS: wrong password"));
		return;
	}

	if (client.isAuthenticated())
	{
		sendToClient(client.getFd(), ":ft_irc 462 :You may not reregister\r\n");
		return;
	}

	if (args[1] != pass)
	{
		sendToClient(client.getFd(), ":ft_irc 464 :Password incorrect\r\n");
		return;
	}

	std::cout << "Password accepted for potential client" << std::endl;
}

void Command::handleNick(Client &client, const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), ":ft_irc 431 :No nickname given\r\n");
		return;
	}
	// change the nickname of a client
	client.setNickname(args[1]);
	std::cout << "new NICK " + client.getNickname() << std::endl;
}

void Command::handleUser(Client &client, const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), ":ft_irc 461 USER :Not enough parameters\r\n");
		return;
	}
	// change the username of a client
	client.setUsername(args[1]);
	std::cout << "new USER " + client.getUsername() << std::endl;
}

void Command::handleJoin(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	// check if the channel exist if yes check the invites / join settings otherwise create a channel
	std::string channelName(args[1]);
	if (!channels[channelName])
	{
		channels[channelName] = new Channel(channelName);
		std::cout << client.getNickname() + " created: " + channels[channelName]->getChannelName() << std::endl;
	}
	channels[channelName]->addClient(&client);
	std::cout << client.getNickname() + " joined: " + channels[channelName]->getChannelName() << std::endl;
}

void Command::handlePrivmsg(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	if (args.size() < 3) // Need at least: PRIVMSG <target> :<message>
		return;

	std::string target = args[1]; // channel name

	// get the messsage after ':'
	std::string message = "";
	for (size_t i = 2; i < args.size(); i++)
	{
		if (i > 2)
			message += " ";
		message += args[i];
	}
	// Remove ':'
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);

	// Build IRC message
	std::string formattedMsg = ":" + client.getNickname() + "!" +
							   client.getUsername() + "@localhost PRIVMSG " +
							   target + " :" + message + "\r\n";

	// Send to channel (if it exists)
	if (channels.find(target) != channels.end())
		channels[target]->sendMessageToClients(client.getFd(), formattedMsg);
}

// if a message is sent to a channel need to send it to all participant of that channel
// to get historique need to have std::vector<string> that got all previous message ready to send to a new joining client
// forward message format <nick>!<user>@<host> PRIVMSG #channel :hello everyone!\r\n

// to kick command KICK <channel> <user> [:<reason>] check op right from the user.fd

void Command::sendWelcomeMessage(Client &client)
{
	std::string nick = client.getNickname();
	sendToClient(client.getFd(), ":ft_irc 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!\r\n");
	sendToClient(client.getFd(), ":ft_irc 002 " + nick + " :Your host is ft_irc, running version 1.0\r\n");
	sendToClient(client.getFd(), ":ft_irc 003 " + nick + " :This server was created today\r\n");
}

void Command::handleLeave(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)client;
	(void)channels;
	(void)args;
}
void Command::handleMode(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)client;
	(void)channels;
	(void)args;
}
void Command::handleTopic(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)client;
	(void)channels;
	(void)args;
}
void Command::handleQuit(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)client;
	(void)channels;
	(void)args;
}
void Command::handleInvite(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)client;
	(void)channels;
	(void)args;
}