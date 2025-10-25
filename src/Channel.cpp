#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Client.hpp"

#include <map>
#include <string>
#include <iostream>
#include <iostream>

// Constructor & Destructor
Channel::Channel(std::string channelName)
    :   _channelName(channelName)
{ std::cout << "Channel constructor called" << std::endl; }
{ std::cout << "Channel constructor called" << std::endl; }

Channel::~Channel()
{ std::cout << "Channel destructor called" << std::endl; }
{ std::cout << "Channel destructor called" << std::endl; }

// Getters
std::string Channel::getChannelName() const
{ return (_channelName); }
// Getters
std::string Channel::getChannelName() const
{ return (_channelName); }

// Methodes
void    Channel::addClient(Client* client)
{ _clientList[client->getFd()] = client; }