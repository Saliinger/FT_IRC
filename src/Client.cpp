#include "../include/Client.hpp"

#include <iostream>
#include <string>

Client::Client(int fd)
    :   _fd(fd)
{
    std::cout   << "Constructor called" << std::endl;
}

Client::~Client()
{
    std::cout << "Destructor called" << std::endl;
}

// Getters & Setters
int Client::getFd()
{
    return (this->_fd);
}

std::string Client::getNickname()
{
    return (this->_nickname);
}

std::string Client::getUsername()
{
    return (this->_username);
}

void    Client::setNickname(std::string nickname)
{
    this->_nickname = nickname;
}

void    Client::setUsername(std::string username)
{
    this->_username = username;
}
