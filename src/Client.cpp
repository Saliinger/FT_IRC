#include "../include/Client.hpp"
#include "../include/Channel.hpp"

#include <iostream>
#include <string>

// Constructor & Destructor
Client::Client(int fd)
    :   _fd(fd),
        _isAuth(false)
{ std::cout   << "Client constructor called" << std::endl; }

Client::~Client()
{ std::cout << "Client destructor called" << std::endl; }

// Getters
int Client::getFd() const
{ return (_fd); }

bool Client::isAuthenticated() const
{ return (_isAuth); }

std::string Client::getNickname() const
{ return (_nickname); }

std::string Client::getUsername() const
{ return (_username); }

// Setters
void    Client::setNickname(const std::string& nickname)
{ _nickname = nickname; }

void    Client::setUsername(const std::string& username)
{ _username = username; }

//Methodes
void    Client::authenticate()
{ _isAuth = true; }

void    Client::joinChannel(Channel* channel)
{
    std::cout << "Client: " << _username << " joined channel: " << channel->getChannelName() << std::endl;
    _channelList[channel->getChannelName()] = channel;
    channel->addClient(this);
}

void    Client::leaveChannel(Channel* channel)
{
    std::cout << "Client: " << _username << " left channel: " << channel->getChannelName() << std::endl;
    _channelList.erase(channel->getChannelName());
    channel->removeClient(this);
}