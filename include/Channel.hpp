#pragma once

#include "Modes.hpp"

#include <string>
#include <map>
#include <list>

class Client;

class Channel
{
	private:
		std::string					_channelName;
		std::map<int, Client *>		_clientList;
		std::map<int, Client *>		_operatorList;
		Modes						_modes;

	public:
		Channel(std::string channelName);
		~Channel();

		// Getters & Setters
		std::string						getChannelName() const;
		const std::map<int, Client *>&	getClients() const;
		const std::map<int, Client *>&	getOperators() const;
		bool							getChannelMode(ChannelModes mode) const;
		void							setChannelMode(ChannelModes mode, bool b);

		// Methodes
		void		addClient(Client* client);
		void		removeClient(Client* client);
		bool		setOperator(Client* client);
		bool		removeOperator(Client* client);
		bool 		isOperator(Client* client) const;
		// topic]
};