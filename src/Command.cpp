#include "../include/Command.hpp"
#include <unistd.h>

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
	_unauthCommand["USER"] = &Command::handleUser;
	// Note: PASS and NICK are handled separately

	// Authenticated commands - channel operations
	_authCommand["JOIN"] = &Command::handleJoin;
	_authCommand["PRIVMSG"] = &Command::handlePrivmsg;
	_authCommand["QUIT"] = &Command::handleQuit;
	_authCommand["LEAVE"] = &Command::handleLeave;
	_authCommand["PART"] = &Command::handlePart;
	_authCommand["KICK"] = &Command::handleKick;
	_authCommand["MODE"] = &Command::handleMode;
	_authCommand["TOPIC"] = &Command::handleTopic;
	_authCommand["INVITE"] = &Command::handleInvite;
	// Note: NICK and USER handled specially in handleAuthenticatedCommand
}

void Command::run(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, std::string &command, const std::string pass)
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
		handleUnauthenticatedCommand(clients, client, cmd, tokens, pass);
		checkAndCompleteRegistration(clients, client, channels, command, pass);
	}
	else
	{
		handleAuthenticatedCommand(clients, client, channels, cmd, tokens);
	}
}

void Command::handleUnauthenticatedCommand(std::map<int, Client *> &clients, Client &client, const std::string &cmd, const std::vector<std::string> &tokens, const std::string &pass)
{
	// Special case for PASS command (needs password parameter)
	if (cmd == "PASS")
	{
		handlePass(client, tokens, pass);
		return;
	}

	// Special case for NICK (needs clients map)
	if (cmd == "NICK")
	{
		handleNick(clients, client, tokens);
		return;
	}

	// Look up other unauthenticated commands
	std::map<std::string, UnauthHandler>::iterator it = _unauthCommand.find(cmd);
	if (it == _unauthCommand.end())
		return; // Command not found - silently ignore

	// Call member function pointer - syntax: (this->*functionPointer)(args)
	(this->*(it->second))(client, tokens);
}

void Command::handleAuthenticatedCommand(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, std::string &cmd, std::vector<std::string> &tokens)
{
	// Special cases for commands that don't need channel map
	if (cmd == "NICK")
	{
		handleNick(clients, client, tokens);
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
	(this->*(it->second))(clients, client, channels, tokens);
}

void Command::checkAndCompleteRegistration(
	std::map<int, Client *> &clients,
	Client &client,
	std::map<std::string, Channel *> &channels,
	std::string &command,
	const std::string &pass)
{
	if (!client.getNickname().empty() && !client.getUsername().empty())
	{
		client.authenticate();
		sendWelcomeMessage(client);
		run(clients, client, channels, command, pass);
	}
}

// base setup
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

void Command::handleNick(std::map<int, Client *> &clients, Client &client, const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), ":ft_irc 431 :No nickname given\r\n");
		return;
	}

	std::string newNick = args[1];

	// Check if nickname is already in use by another client
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getFd() != client.getFd() && it->second->getNickname() == newNick)
		{
			sendToClient(client.getFd(), ":ft_irc " ERR_NICKNAMEINUSE " " + (client.getNickname().empty() ? "*" : client.getNickname()) + " " + newNick + " :Nickname is already in use\r\n");
			return;
		}
	}

	// change the nickname of a client
	client.setNickname(newNick);
	std::cout << "new NICK " + client.getNickname() << std::endl;
}

// channel related commands
void Command::handleJoin(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)clients;
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "JOIN :Not enough parameters"));
		return;
	}

	std::string channelName(args[1]);

	// Validate channel name starts with #
	if (channelName.empty() || channelName[0] != '#')
	{
		sendToClient(client.getFd(), formatReply(ERR_NOSUCHCHANNEL, client.getNickname(), channelName + " :No such channel"));
		return;
	}

	bool isNewChannel = false;
	if (!channels[channelName])
	{
		channels[channelName] = new Channel(channelName);
		isNewChannel = true;
		std::cout << client.getNickname() + " created: " + channels[channelName]->getChannelName() << std::endl;
	}

	Channel *chan = channels[channelName];

	// Check if already in channel
	if (chan->getClients().count(client.getFd()))
	{
		return; // Already in channel, silently ignore
	}

	chan->addClient(&client);

	// Make creator operator
	if (isNewChannel)
	{
		chan->setOperator(&client);
	}

	// Send JOIN confirmation to the joining client
	std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + channelName + "\r\n";
	sendToClient(client.getFd(), joinMsg);

	// Broadcast JOIN to all other members in the channel
	chan->sendMessageToClients(client.getFd(), joinMsg);

	// Send topic if exists
	if (!chan->getTopic().empty())
	{
		sendToClient(client.getFd(), ":ft_irc " RPL_TOPIC " " + client.getNickname() + " " + channelName + " :" + chan->getTopic() + "\r\n");
	}
	else
	{
		sendToClient(client.getFd(), ":ft_irc " RPL_NOTOPIC " " + client.getNickname() + " " + channelName + " :No topic is set\r\n");
	}

	// Send NAMES list (users in channel)
	std::string namesList = ":" + client.getNickname();
	const std::map<int, Client *> &clientsInChan = chan->getClients();
	for (std::map<int, Client *>::const_iterator it = clientsInChan.begin(); it != clientsInChan.end(); ++it)
	{
		if (chan->isOperator(it->second))
			namesList += " @" + it->second->getNickname();
		else
			namesList += " " + it->second->getNickname();
	}
	sendToClient(client.getFd(), ":ft_irc " RPL_NAMREPLY " " + client.getNickname() + " = " + channelName + " " + namesList + "\r\n");
	sendToClient(client.getFd(), ":ft_irc " RPL_ENDOFNAMES " " + client.getNickname() + " " + channelName + " :End of NAMES list\r\n");

	std::cout << client.getNickname() + " joined: " + chan->getChannelName() << std::endl;
}

void Command::handlePrivmsg(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	if (args.size() < 3) // Need at least: PRIVMSG <target> :<message>
	{
		sendToClient(client.getFd(), formatReply(ERR_NORECIPIENT, client.getNickname(), "PRIVMSG :No recipient given"));
		return;
	}

	std::string target = args[1];

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

	if (message.empty())
	{
		sendToClient(client.getFd(), formatReply(ERR_NOTEXTTOSEND, client.getNickname(), ":No text to send"));
		return;
	}

	// Build IRC message
	std::string formattedMsg = ":" + client.getNickname() + "!" +
							   client.getUsername() + "@localhost PRIVMSG " +
							   target + " :" + message + "\r\n";

	// Check if target is a channel (starts with #)
	if (!target.empty() && target[0] == '#')
	{
		// Send to channel (if it exists)
		if (channels.find(target) != channels.end())
		{
			Channel *chan = channels[target];
			// Check if sender is in channel
			if (!chan->getClients().count(client.getFd()))
			{
				sendToClient(client.getFd(), formatReply(ERR_CANNOTSENDTOCHAN, client.getNickname(), target + " :Cannot send to channel"));
				return;
			}
			chan->sendMessageToClients(client.getFd(), formattedMsg);
		}
		else
		{
			sendToClient(client.getFd(), formatReply(ERR_NOSUCHCHANNEL, client.getNickname(), target + " :No such channel"));
		}
	}
	else
	{
		// Private message to user
		Client *targetClient = NULL;
		for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->second->getNickname() == target)
			{
				targetClient = it->second;
				break;
			}
		}

		if (!targetClient)
		{
			sendToClient(client.getFd(), formatReply(ERR_NOSUCHNICK, client.getNickname(), target + " :No such nick/channel"));
			return;
		}

		sendToClient(targetClient->getFd(), formattedMsg);
	}
}

void Command::sendWelcomeMessage(Client &client)
{
	std::string nick = client.getNickname();
	sendToClient(client.getFd(), ":ft_irc 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!\r\n");
	sendToClient(client.getFd(), ":ft_irc 002 " + nick + " :Your host is ft_irc, running version 1.0\r\n");
	sendToClient(client.getFd(), ":ft_irc 003 " + nick + " :This server was created today\r\n");
}

void Command::handleLeave(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)clients;
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "LEAVE :Not enough parameters"));
		return;
	}
	// leave channel
	std::string channelName = args[1];
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatReply(ERR_NOSUCHCHANNEL, client.getNickname(), channelName + " :No such channel"));
		return;
	}

	Channel *chan = channels[channelName];
	if (!chan->getClients().count(client.getFd()))
	{
		sendToClient(client.getFd(), formatReply(ERR_NOTONCHANNEL, client.getNickname(), channelName + " :You're not on that channel"));
		return;
	}

	std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channelName + "\r\n";
	sendToClient(client.getFd(), partMsg);
	chan->sendMessageToClients(client.getFd(), partMsg);
	chan->removeClient(&client);
}

void Command::handlePart(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)clients;
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "PART :Not enough parameters"));
		return;
	}

	std::string channelName = args[1];
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatReply(ERR_NOSUCHCHANNEL, client.getNickname(), channelName + " :No such channel"));
		return;
	}

	Channel *chan = channels[channelName];
	if (!chan->getClients().count(client.getFd()))
	{
		sendToClient(client.getFd(), formatReply(ERR_NOTONCHANNEL, client.getNickname(), channelName + " :You're not on that channel"));
		return;
	}

	// Optional part message
	std::string reason;
	if (args.size() >= 3)
	{
		for (size_t i = 2; i < args.size(); ++i)
		{
			if (i > 2)
				reason += " ";
			reason += args[i];
		}
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}

	std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channelName;
	if (!reason.empty())
		partMsg += " :" + reason;
	partMsg += "\r\n";

	sendToClient(client.getFd(), partMsg);
	chan->sendMessageToClients(client.getFd(), partMsg);
	chan->removeClient(&client);
}

void Command::handleKick(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)clients;
	// KICK #channel nickname :reason
	if (args.size() < 3)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "KICK :Not enough parameters"));
		return;
	}

	std::string channelName = args[1];
	std::string targetNick = args[2];

	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatReply(ERR_NOSUCHCHANNEL, client.getNickname(), channelName + " :No such channel"));
		return;
	}

	Channel *chan = channels[channelName];

	// Check if kicker is on channel
	if (!chan->getClients().count(client.getFd()))
	{
		sendToClient(client.getFd(), formatReply(ERR_NOTONCHANNEL, client.getNickname(), channelName + " :You're not on that channel"));
		return;
	}

	// Check if kicker is operator
	if (!chan->isOperator(&client))
	{
		sendToClient(client.getFd(), formatReply(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName + " :You're not channel operator"));
		return;
	}

	// Find target client in channel
	Client *targetClient = NULL;
	const std::map<int, Client *> &clientsInChan = chan->getClients();
	for (std::map<int, Client *>::const_iterator it = clientsInChan.begin(); it != clientsInChan.end(); ++it)
	{
		if (it->second->getNickname() == targetNick)
		{
			targetClient = it->second;
			break;
		}
	}

	if (!targetClient)
	{
		sendToClient(client.getFd(), formatReply(ERR_USERNOTINCHANNEL, client.getNickname(), targetNick + " " + channelName + " :They aren't on that channel"));
		return;
	}

	// Build kick message with optional reason
	std::string reason;
	if (args.size() >= 4)
	{
		for (size_t i = 3; i < args.size(); ++i)
		{
			if (i > 3)
				reason += " ";
			reason += args[i];
		}
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}
	else
	{
		reason = client.getNickname();
	}

	std::string kickMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";

	// Send to all including the kicked user
	sendToClient(targetClient->getFd(), kickMsg);
	chan->sendMessageToClients(targetClient->getFd(), kickMsg);

	// Remove from channel
	chan->removeClient(targetClient);
}

void Command::handleMode(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)clients;
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "MODE :Not enough parameters"));
		return;
	}

	std::string channelName = args[1];
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatReply(ERR_NOSUCHCHANNEL, client.getNickname(), channelName + " :No such channel"));
		return;
	}

	Channel *chan = channels[channelName];

	// If only channel supplied, return current channel modes
	if (args.size() == 2)
	{
		std::string modeStr = "+";
		if (chan->getChannelMode(MODE_T))
			modeStr += "t";
		if (chan->getChannelMode(MODE_I))
			modeStr += "i";
		if (chan->getChannelMode(MODE_K))
			modeStr += "k";
		if (chan->getChannelMode(MODE_L))
			modeStr += "l";

		// If no channel modes, still respond with +
		sendToClient(client.getFd(), ":ft_irc " RPL_CHANNELMODEIS " " + client.getNickname() + " " + channelName + " " + modeStr + "\r\n");
		return;
	}

	// Modes with parameters or toggles
	std::string modeArg = args[2];
	if (modeArg.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_UNKNOWNMODE, client.getNickname(), std::string(1, modeArg.empty() ? ' ' : modeArg[0]) + " :is unknown mode char to me for " + channelName));
		return;
	}

	char sign = modeArg[0];
	char flag = modeArg[1];

	// User operator mode (+o / -o) requires a target nick
	if (flag == 'o')
	{
		if (args.size() < 4)
		{
			sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "MODE :Not enough parameters"));
			return;
		}

		// Only channel operators can change other user's operator status
		if (!chan->isOperator(&client))
		{
			sendToClient(client.getFd(), formatReply(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName + " :You're not channel operator"));
			return;
		}

		std::string targetNick = args[3];
		Client *targetClient = NULL;
		const std::map<int, Client *> &clients = chan->getClients();
		for (std::map<int, Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->second->getNickname() == targetNick)
			{
				targetClient = it->second;
				break;
			}
		}

		if (!targetClient)
		{
			sendToClient(client.getFd(), formatReply(ERR_USERNOTINCHANNEL, client.getNickname(), targetNick + " " + channelName + " :They aren't on that channel"));
			return;
		}

		bool res = false;
		std::string flagStr(1, flag);
		if (sign == '+')
		{
			res = chan->setOperator(targetClient);
		}
		else if (sign == '-')
		{
			res = chan->removeOperator(targetClient);
		}
		else
		{
			sendToClient(client.getFd(), formatReply(ERR_UNKNOWNMODE, client.getNickname(), std::string(1, flag) + " :is unknown mode char to me for " + channelName));
			return;
		}

		// Notify channel of the mode change
		std::string formattedMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channelName + " " + std::string(1, sign) + flagStr + " " + targetNick + "\r\n";
		// send to all channel members (including setter)
		chan->sendMessageToClients(-1, formattedMsg);
		return;
	}

	// Channel-level boolean modes (example: t)
	if (flag == 't' || flag == 'i' || flag == 'k' || flag == 'l')
	{
		// Only channel operators can set channel modes
		if (!chan->isOperator(&client))
		{
			sendToClient(client.getFd(), formatReply(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName + " :You're not channel operator"));
			return;
		}

		ChannelModes modeEnum;
		if (flag == 't')
			modeEnum = MODE_T;
		else if (flag == 'i')
			modeEnum = MODE_I;
		else if (flag == 'k')
			modeEnum = MODE_K;
		else
			modeEnum = MODE_L;

		bool set = (sign == '+');
		chan->setChannelMode(modeEnum, set);

		std::string formattedMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channelName + " " + std::string(1, sign) + std::string(1, flag) + "\r\n";
		chan->sendMessageToClients(-1, formattedMsg);
		return;
	}

	// Unknown mode character
	sendToClient(client.getFd(), formatReply(ERR_UNKNOWNMODE, client.getNickname(), std::string(1, flag) + " :is unknown mode char to me for " + channelName));
}

void Command::handleTopic(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)clients;
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "TOPIC :Not enough parameters"));
		return;
	}

	std::string channelName = args[1];
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatReply(ERR_NOSUCHCHANNEL, client.getNickname(), channelName + " :No such channel"));
		return;
	}

	Channel *chan = channels[channelName];

	// Check if user is on channel
	if (!chan->getClients().count(client.getFd()))
	{
		sendToClient(client.getFd(), formatReply(ERR_NOTONCHANNEL, client.getNickname(), channelName + " :You're not on that channel"));
		return;
	}

	// If no topic parameter, return current topic
	if (args.size() == 2)
	{
		if (chan->getTopic().empty())
		{
			sendToClient(client.getFd(), ":ft_irc " RPL_NOTOPIC " " + client.getNickname() + " " + channelName + " :No topic is set\r\n");
		}
		else
		{
			sendToClient(client.getFd(), ":ft_irc " RPL_TOPIC " " + client.getNickname() + " " + channelName + " :" + chan->getTopic() + "\r\n");
		}
		return;
	}

	// Setting topic - check if operator
	if (!chan->isOperator(&client))
	{
		sendToClient(client.getFd(), formatReply(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName + " :You're not channel operator"));
		return;
	}

	// reconstruction of topic from args after :
	std::string topic;
	for (size_t i = 2; i < args.size(); i++)
	{
		if (i > 2)
			topic += " ";
		topic += args[i];
	}
	if (!topic.empty() && topic[0] == ':')
		topic = topic.substr(1);

	chan->setTopic(topic);

	// Broadcast topic change to all channel members
	std::string topicMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost TOPIC " + channelName + " :" + topic + "\r\n";
	sendToClient(client.getFd(), topicMsg);
	chan->sendMessageToClients(client.getFd(), topicMsg);
}

void Command::handleInvite(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	// INVITE <nickname> <channel>
	if (args.size() < 3)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "INVITE :Not enough parameters"));
		return;
	}

	std::string targetNick = args[1];
	std::string channelName = args[2];

	// Check if channel exists
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatReply(ERR_NOSUCHCHANNEL, client.getNickname(), channelName + " :No such channel"));
		return;
	}

	Channel *chan = channels[channelName];

	// Check if inviter is on the channel
	if (!chan->getClients().count(client.getFd()))
	{
		sendToClient(client.getFd(), formatReply(ERR_NOTONCHANNEL, client.getNickname(), channelName + " :You're not on that channel"));
		return;
	}

	// Check if inviter is operator (if channel has invite-only mode, operator check may be needed)
	if (!chan->isOperator(&client))
	{
		sendToClient(client.getFd(), formatReply(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName + " :You're not channel operator"));
		return;
	}

	// Find target client by nickname in server's client map
	Client *targetClient = NULL;
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNickname() == targetNick)
		{
			targetClient = it->second;
			break;
		}
	}

	if (!targetClient)
	{
		sendToClient(client.getFd(), formatReply(ERR_NOSUCHNICK, client.getNickname(), targetNick + " :No such nick/channel"));
		return;
	}

	// Check if target is already on the channel
	if (chan->getClients().count(targetClient->getFd()))
	{
		sendToClient(client.getFd(), formatReply(ERR_USERONCHANNEL, client.getNickname(), targetNick + " " + channelName + " :is already on channel"));
		return;
	}

	// Send RPL_INVITING to inviter
	sendToClient(client.getFd(), ":ft_irc " RPL_INVITING " " + client.getNickname() + " " + targetNick + " " + channelName + "\r\n");

	// Send INVITE message to target client
	std::string inviteMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost INVITE " + targetNick + " :" + channelName + "\r\n";
	sendToClient(targetClient->getFd(), inviteMsg);
}

void Command::handleQuit(std::map<int, Client *> &clients, Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)clients;
	// Build quit message from args (optional)
	std::string reason = "Client Quit";
	if (args.size() >= 2)
	{
		// reconstruct reason (may start with ':')
		reason.clear();
		for (size_t i = 1; i < args.size(); ++i)
		{
			if (i > 1)
				reason += " ";
			reason += args[i];
		}
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}

	std::string quitMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost QUIT :" + reason + "\r\n";

	// Broadcast QUIT to all channels the client is a member of and remove client from them
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel *ch = it->second;
		const std::map<int, Client *> &clients = ch->getClients();
		if (clients.find(client.getFd()) != clients.end())
		{
			// send to others in channel (exclude quitting fd)
			ch->sendMessageToClients(client.getFd(), quitMsg);
			// remove client from channel
			ch->removeClient(&client);
		}
	}

	// mark client unauthenticated
	client.unAuthenticate();

	// Notify the client (optional) then close fd
	sendToClient(client.getFd(), quitMsg);
	close(client.getFd());
}
