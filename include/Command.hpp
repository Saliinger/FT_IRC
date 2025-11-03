#pragma once

#include <iostream>

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Errors.hpp"

class Client;
class Channel;

typedef void (*AuthHandler)(Client &, Channel &, const std::vector<std::string> &);
typedef void (*UnauthHandler)(Client &, const std::vector<std::string> &, const std::string &);

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

	// helper
	void handlePass(Client &client, const std::vector<std::string> &args, const std::string pass);
	void handleNick(Client &client, const std::vector<std::string> &args);
	void handleUser(Client &client, const std::vector<std::string> &args);
	void handleJoin(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);
	void handlePrivmsg(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args);

	// un auth command
	void handleUnauthenticatedCommand(Client &client, const std::string &cmd, const std::vector<std::string> &tokens,
									  const std::string &pass);

	// check user completion
	void checkAndCompleteRegistration(Client &client, std::map<std::string, Channel *> &channels, std::string &command, const std::string &pass);

	// auth commands
	void handleAuthenticatedCommand(Client &client, std::map<std::string, Channel *> &channels, std::string &cmd, std::vector<std::string> &tokens);

	void createCommandMap();

	bool requiresAuthentication(const std::string &cmd);
	void sendWelcomeMessage(Client &client);
};