#pragma once

#include <Client.hpp>
#include <list>
#include <string>

class Channel
{
	private:
		std::string					_channelName;
		std::map<int, Client *>		_clientList;

	public:
		Channel(std::string channelName);
		~Channel();

		// Methodes
		void	addClient(Client& client);
};


