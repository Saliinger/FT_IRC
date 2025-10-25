#include "../include/Command.hpp"

void Command::handleCommand(Client &client, Channel &channel, std::string &command)
{
    (void)channel;
    std::vector<std::string> tokens = split(command, ' ');

    std::string cmd = tokens[0];

    if (cmd == "PASS")
        handlePass(client, tokens);
    else if (cmd == "NICK")
        handleNick(client, tokens);
    else if (cmd == "USER")
        handleUser(client, tokens);
    else if (cmd == "JOIN")
        handleJoin(client, tokens);
    else if (cmd == "PRIVMSG")
        handlePrivmsg(client, tokens);
    else if (cmd == "QUIT")
        std::cout << "leaving" << std::endl; // segv if client leave cause of no handling
}

void Command::handlePass(Client &client, const std::vector<std::string> &args)
{
    (void)client;
    (void)args;
    std::cout << "handler called" << std::endl;
}

void Command::handleNick(Client &client, const std::vector<std::string> &args)
{
    (void)client;
    (void)args;
    std::cout << "handler called" << std::endl;
}

void Command::handleUser(Client &client, const std::vector<std::string> &args)
{
    (void)client;
    (void)args;
    std::cout << "handler called" << std::endl;
}

void Command::handleJoin(Client &client, const std::vector<std::string> &args)
{
    (void)client;
    (void)args;
    std::cout << "handler called" << std::endl;
}

void Command::handlePrivmsg(Client &client, const std::vector<std::string> &args)
{
    (void)client;
    (void)args;
    std::cout << "handler called" << std::endl;
}