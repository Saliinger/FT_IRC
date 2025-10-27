#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Client.hpp"

#include <map>
#include <list>
#include <string>
#include <iostream>
#include <algorithm>

// Constructor & Destructor
Channel::Channel(std::string channelName)
    : _channelName(channelName)
{
    std::cout << "Channel constructor called" << std::endl;
}

Channel::~Channel()
{
    std::cout << "Channel destructor called" << std::endl;
}

// Getters
std::string Channel::getChannelName() const
{
    return (_channelName);
}

const std::map<int, Client *> &Channel::getClients() const
{
    return (_clientList);
}

// Methodes
void Channel::addClient(Client *client)
{
    _clientList[client->getFd()] = client;
}

void Channel::removeClient(Client *client)
{
    _clientList.erase(client->getFd());
}

bool Channel::setOperator(Client *client)
{
    if (_clientList.find(client->getFd()) != _clientList.end())
    {
        _operatorList[client->getFd()] = client;
        return (true);
    }
    return (false);
}

bool Channel::removeOperator(Client *client)
{
    if (_operatorList.find(client->getFd()) != _operatorList.end())
    {
        _operatorList.erase(client->getFd());
        return (true);
    }
    return (false);
}

bool Channel::isOperator(Client *client) const
{
    if (_operatorList.find(client->getFd()) != _operatorList.end())
        return (true);
    return (false);
}

// alexis's functions
void Channel::forwardMessage(int fd, std::string &msg)
{
    std::map<int, Client *>::iterator it = _clientList.begin();
    std::map<int, Client *>::iterator ite = _clientList.end();

    while (it != ite)
    {
        if (it->second->getFd() != fd)
            sendToClient(it->second->getFd(), msg);
        it++;
    }
}