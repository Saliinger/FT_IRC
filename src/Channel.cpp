#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Modes.hpp"
#include "../include/Utils.hpp"

#include <map>
#include <list>
#include <string>
#include <iostream>
#include <algorithm>

// Constructor & Destructor
Channel::Channel(std::string channelName)
    :   _channelName(channelName)
{ std::cout << "Channel constructor called" << std::endl; }

Channel::~Channel()
{ std::cout << "Channel destructor called" << std::endl; }

// Getters
std::string Channel::getChannelName() const
{ return (_channelName); }

const std::map<int, Client *>& Channel::getClients() const
{ return (_clientList); }

const std::map<int, Client *>& Channel::getOperators() const
{ return (_operatorList); }

const std::string&  Channel::getTopic() const
{ return (_topic); }

bool    Channel::getChannelMode(ChannelModes mode) const
{ return _modes.getMode(mode); }

// Setters
void    Channel::setChannelMode(ChannelModes mode, bool b)
{ _modes.setMode(mode, b); }

void    Channel::setTopic(const std::string& topic)
{ _topic = topic; }

// Methodes
void    Channel::addClient(Client* client)
{ _clientList[client->getFd()] = client; }

void    Channel::removeClient(Client* client)
{ 
    if (isOperator(client))
        removeOperator(client);
    _clientList.erase(client->getFd()); 
}

bool    Channel::setOperator(Client* client)
{
    if (_clientList.find(client->getFd()) != _clientList.end())
    {
        _operatorList[client->getFd()] = client;
        return (true);
    }
    return (false);
}

bool    Channel::removeOperator(Client* client)
{
    if (_operatorList.find(client->getFd()) != _operatorList.end())
    {
        _operatorList.erase(client->getFd());
        return (true);
    }
    return (false);
}

bool    Channel::isOperator(Client* client) const
{ 
    if (_operatorList.find(client->getFd()) != _operatorList.end())
        return (true);
    return (false);
} 

void    Channel::sendMessageToClients(int fd, const std::string& message)
{
    std::map<int, Client *>::const_iterator it = _clientList.begin();
    while (it != _clientList.end())
    {
        if (it->first != fd)
            sendToClient(it->first, message);
        it++;
    }
}
