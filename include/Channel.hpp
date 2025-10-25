#pragma once

#include <string>
#include <map>

class Client;

class Channel
{
	private:
		std::string					_channelName;
		std::map<int, Client *>		_clientList;

	public:
		Channel(std::string channelName);
		~Channel();

		// Getters & Setters
		std::string	getChannelName() const;

		// Methodes
		void		addClient(Client* client);
};


