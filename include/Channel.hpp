#pragma once

#include <string>
#include <map>
#include <list>

#include "Utils.hpp"

class Client;
#include <map>

class Client;

class Channel
{
	private:
		std::string					_channelName;
		std::map<int, Client *>		_clientList;
		std::map<int, Client *>		_operatorList;

	public:
		Channel(std::string channelName);
		~Channel();

		// Getters & Setters
		std::string						getChannelName() const;
		const std::map<int, Client *>&	getClients() const;

		// Getters & Setters
		std::string	getChannelName() const;

		// Methodes
		void		addClient(Client* client);
		void		removeClient(Client* client);
		bool		setOperator(Client* client);
		bool		removeOperator(Client* client);
		bool 		isOperator(Client* client) const;
		// get operators
		// remove operator
		// modes
		// topic

		// alexis's functions
		void		forwardMessage(int fd, std::string &msg);
};