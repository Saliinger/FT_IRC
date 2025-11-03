#pragma once

#include <iostream>

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Errors.hpp"

class Client;
class Channel;

class Command;

// Member function pointers that match actual handler signatures
typedef void (Command::*AuthHandler)(Client &, std::map<std::string, Channel *> &, const std::vector<std::string> &);
typedef void (Command::*UnauthHandler)(Client &, const std::vector<std::string> &);

class Command
{
public:
	Command();
	Command(const Command &src);
	Command &operator=(const Command &src);
	~Command();

	void run(Client &client, std::map<std::string, Channel *> &channels, std::string &command, const std::string pass);

private:
	// var
	std::map<std::string, AuthHandler> _authCommand;
	std::map<std::string, UnauthHandler> _unauthCommand;

	// init
	void createCommandMap();

	// helper
	void handlePass(Client &client, const std::vector<std::string> &args, const std::string pass);
	void handleNick(Client &client, const std::vector<std::string> &args);
	void handleUser(Client &client, const std::vector<std::string> &args);
	void handleJoin(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handlePrivmsg(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handleLeave(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handleMode(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handleTopic(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handleQuit(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handleKick(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handleInvite(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handleUnauthenticatedCommand(Client &client, const std::string &cmd, const std::vector<std::string> &tokens,
									  const std::string &pass);

	// check user completion
	void checkAndCompleteRegistration(Client &client, std::map<std::string, Channel *> &channels, std::string &command, const std::string &pass);

	// auth commands
	void handleAuthenticatedCommand(Client &client, std::map<std::string, Channel *> &channels, std::string &cmd, std::vector<std::string> &tokens);

	void sendWelcomeMessage(Client &client);
};