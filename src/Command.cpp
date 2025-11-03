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
	_authCommand["PART"] = &Command::handleLeave;
	_authCommand["KICK"] = &Command::handleKick;
	_authCommand["INVITE"] = &Command::handleInvite;
	_authCommand["TOPIC"] = &Command::handleTopic;
	_authCommand["MODE"] = &Command::handleMode;
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

// ============================================================================
// PART - Quitter un channel
// ============================================================================
void Command::handleLeave(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "PART :Not enough parameters"));
		return;
	}

	std::string channelName = args[1];
	
	// Vérifier si le channel existe
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatError(ERR_NOSUCHCHANNEL, client.getNickname(), channelName, ":No such channel"));
		return;
	}

	Channel *channel = channels[channelName];
	
	// Vérifier si le client est dans le channel
	const std::map<int, Client *>& clientList = channel->getClients();
	if (clientList.find(client.getFd()) == clientList.end())
	{
		sendToClient(client.getFd(), formatError(ERR_NOTONCHANNEL, client.getNickname(), channelName, ":You're not on that channel"));
		return;
	}

	// Message de départ aux autres membres
	std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channelName + "\r\n";
	channel->sendMessageToClients(-1, partMsg); // -1 pour envoyer à tous

	// Retirer le client du channel
	channel->removeClient(&client);
	std::cout << client.getNickname() << " left " << channelName << std::endl;
}

// ============================================================================
// KICK - Éjecter un utilisateur du channel (opérateur uniquement)
// ============================================================================
void Command::handleKick(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	// KICK #channel nickname :reason
	if (args.size() < 3)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "KICK :Not enough parameters"));
		return;
	}

	std::string channelName = args[1];
	std::string targetNick = args[2];
	std::string reason = "Kicked by operator";
	
	if (args.size() > 3)
	{
		reason = args[3];
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}

	// Vérifier si le channel existe
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatError(ERR_NOSUCHCHANNEL, client.getNickname(), channelName, ":No such channel"));
		return;
	}

	Channel *channel = channels[channelName];

	// Vérifier si le client est opérateur
	if (!channel->isOperator(&client))
	{
		sendToClient(client.getFd(), formatError(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName, ":You're not channel operator"));
		return;
	}

	// Trouver le client cible
	const std::map<int, Client *>& clientList = channel->getClients();
	Client *targetClient = NULL;
	for (std::map<int, Client *>::const_iterator it = clientList.begin(); it != clientList.end(); ++it)
	{
		if (it->second->getNickname() == targetNick)
		{
			targetClient = it->second;
			break;
		}
	}

	if (!targetClient)
	{
		sendToClient(client.getFd(), formatError(ERR_USERNOTINCHANNEL, client.getNickname(), targetNick + " " + channelName, ":They aren't on that channel"));
		return;
	}

	// Message de kick à tous les membres
	std::string kickMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
	channel->sendMessageToClients(-1, kickMsg);

	// Retirer le client du channel
	channel->removeClient(targetClient);
	std::cout << client.getNickname() << " kicked " << targetNick << " from " << channelName << std::endl;
}

// ============================================================================
// INVITE - Inviter un utilisateur dans un channel (opérateur uniquement en mode +i)
// ============================================================================
void Command::handleInvite(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	// INVITE nickname #channel
	if (args.size() < 3)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "INVITE :Not enough parameters"));
		return;
	}

	std::string targetNick = args[1];
	std::string channelName = args[2];

	// Vérifier si le channel existe
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatError(ERR_NOSUCHCHANNEL, client.getNickname(), channelName, ":No such channel"));
		return;
	}

	Channel *channel = channels[channelName];

	// Vérifier si le client est dans le channel
	const std::map<int, Client *>& clientList = channel->getClients();
	if (clientList.find(client.getFd()) == clientList.end())
	{
		sendToClient(client.getFd(), formatError(ERR_NOTONCHANNEL, client.getNickname(), channelName, ":You're not on that channel"));
		return;
	}

	// Si le mode +i est actif, seuls les opérateurs peuvent inviter
	if (channel->getChannelMode(MODE_I) && !channel->isOperator(&client))
	{
		sendToClient(client.getFd(), formatError(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName, ":You're not channel operator"));
		return;
	}

	// TODO: Trouver le client cible dans la liste globale des clients (pas implémenté ici)
	// Pour l'instant, on envoie juste une confirmation
	sendToClient(client.getFd(), formatReply(RPL_INVITING, client.getNickname(), targetNick + " " + channelName));
	std::cout << client.getNickname() << " invited " << targetNick << " to " << channelName << std::endl;
}

// ============================================================================
// TOPIC - Afficher ou changer le topic du channel
// ============================================================================
void Command::handleTopic(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	// TOPIC #channel :new topic
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "TOPIC :Not enough parameters"));
		return;
	}

	std::string channelName = args[1];

	// Vérifier si le channel existe
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatError(ERR_NOSUCHCHANNEL, client.getNickname(), channelName, ":No such channel"));
		return;
	}

	Channel *channel = channels[channelName];

	// Si pas de topic fourni, afficher le topic actuel
	if (args.size() == 2)
	{
		std::string topic = channel->getTopic();
		if (topic.empty())
			sendToClient(client.getFd(), formatError(RPL_NOTOPIC, client.getNickname(), channelName, ":No topic is set"));
		else
			sendToClient(client.getFd(), formatError(RPL_TOPIC, client.getNickname(), channelName, ":" + topic));
		return;
	}

	// Changer le topic
	std::string newTopic = args[2];
	if (!newTopic.empty() && newTopic[0] == ':')
		newTopic = newTopic.substr(1);

	// Vérifier si le mode +t est actif (seuls les opérateurs peuvent changer le topic)
	if (channel->getChannelMode(MODE_T) && !channel->isOperator(&client))
	{
		sendToClient(client.getFd(), formatError(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName, ":You're not channel operator"));
		return;
	}

	// Changer le topic
	channel->setTopic(newTopic);

	// Notifier tous les membres
	std::string topicMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost TOPIC " + channelName + " :" + newTopic + "\r\n";
	channel->sendMessageToClients(-1, topicMsg);
	
	std::cout << client.getNickname() << " changed topic of " << channelName << " to: " << newTopic << std::endl;
}

// ============================================================================
// MODE - Gérer les modes du channel
// ============================================================================
void Command::handleMode(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	// MODE #channel [+/-][i/t/k/o/l] [parameters]
	if (args.size() < 2)
	{
		sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "MODE :Not enough parameters"));
		return;
	}

	std::string channelName = args[1];

	// Vérifier si le channel existe
	if (channels.find(channelName) == channels.end())
	{
		sendToClient(client.getFd(), formatError(ERR_NOSUCHCHANNEL, client.getNickname(), channelName, ":No such channel"));
		return;
	}

	Channel *channel = channels[channelName];

	// Si pas de mode fourni, afficher les modes actuels
	if (args.size() == 2)
	{
		std::string modes = "+";
		if (channel->getChannelMode(MODE_I)) modes += "i";
		if (channel->getChannelMode(MODE_T)) modes += "t";
		if (channel->getChannelMode(MODE_K)) modes += "k";
		if (channel->getChannelMode(MODE_L)) modes += "l";
		sendToClient(client.getFd(), formatError(RPL_CHANNELMODEIS, client.getNickname(), channelName, modes));
		return;
	}

	// Vérifier si le client est opérateur
	if (!channel->isOperator(&client))
	{
		sendToClient(client.getFd(), formatError(ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName, ":You're not channel operator"));
		return;
	}

	// Parser les modes
	std::string modeString = args[2];
	bool adding = true;

	for (size_t i = 0; i < modeString.length(); i++)
	{
		char mode = modeString[i];

		if (mode == '+')
			adding = true;
		else if (mode == '-')
			adding = false;
		else if (mode == 'i')
			channel->setChannelMode(MODE_I, adding);
		else if (mode == 't')
			channel->setChannelMode(MODE_T, adding);
		else if (mode == 'k')
			channel->setChannelMode(MODE_K, adding);
		else if (mode == 'l')
			channel->setChannelMode(MODE_L, adding);
		else if (mode == 'o')
		{
			// Gérer l'opérateur : MODE #channel +o nickname
			if (args.size() < 4)
			{
				sendToClient(client.getFd(), formatReply(ERR_NEEDMOREPARAMS, client.getNickname(), "MODE +o :Not enough parameters"));
				continue;
			}

			std::string targetNick = args[3];
			const std::map<int, Client *>& clientList = channel->getClients();
			Client *targetClient = NULL;

			for (std::map<int, Client *>::const_iterator it = clientList.begin(); it != clientList.end(); ++it)
			{
				if (it->second->getNickname() == targetNick)
				{
					targetClient = it->second;
					break;
				}
			}

			if (!targetClient)
			{
				sendToClient(client.getFd(), formatError(ERR_USERNOTINCHANNEL, client.getNickname(), targetNick + " " + channelName, ":They aren't on that channel"));
				continue;
			}

			if (adding)
				channel->setOperator(targetClient);
			else
				channel->removeOperator(targetClient);
		}
		else
		{
			sendToClient(client.getFd(), formatError(ERR_UNKNOWNMODE, client.getNickname(), std::string(1, mode), ":is unknown mode char to me"));
		}
	}

	// Confirmer le changement de mode
	std::string modeMsg = ":" + client.getNickname() + " MODE " + channelName + " " + modeString + "\r\n";
	channel->sendMessageToClients(-1, modeMsg);
	
	std::cout << client.getNickname() << " set mode " << modeString << " on " << channelName << std::endl;
}

// ============================================================================
// QUIT - Déconnexion du serveur
// ============================================================================
void Command::handleQuit(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
	(void)channels;
	std::string quitMsg = "Client quit";
	
	if (args.size() > 1)
	{
		quitMsg = args[1];
		if (!quitMsg.empty() && quitMsg[0] == ':')
			quitMsg = quitMsg.substr(1);
	}

	sendToClient(client.getFd(), "ERROR :Closing connection (" + quitMsg + ")\r\n");
	std::cout << client.getNickname() << " quit: " << quitMsg << std::endl;
	
	// TODO: Fermer la connexion et nettoyer le client
}