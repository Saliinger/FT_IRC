#include "../include/Channel.hpp"

#include <map>
#include <string>

Channel::Channel(std::string channelName)
    :   _channelName(channelName)
{

}

Channel::~Channel()
{

}

// Methodes
void    Channel::addClient(Client& client)
{
    this->_clientList[client.getFd()] = &client;
}