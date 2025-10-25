#pragma once

#include <iostream>

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Command
{
public:
    static void handleCommand(Client &client, Channel &channel, std::string &command);
    static void handlePass(Client &client, const std::vector<std::string> &args);
    static void handleNick(Client &client, const std::vector<std::string> &args);
    static void handleUser(Client &client, const std::vector<std::string> &args);
    static void handleJoin(Client &client, const std::vector<std::string> &args);
    static void handlePrivmsg(Client &client, const std::vector<std::string> &args);

private:
};