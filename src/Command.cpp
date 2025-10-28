#include "../include/Command.hpp"

void Command::handleCommand(Client &client, std::map<std::string, Channel *> channels, std::string &command, std::string &pass)
{
    std::vector<std::string> tokens = split(command, ' ');
    std::string cmd = tokens[0];

    if (cmd == "PING")
        sendToClient(client.getFd(), "PONG :ft_irc\r\n");
    if (!client.isAuthenticated())
    { // handle base setup before anything
        if (cmd == "WHOIS")
            std::cout << "don't know" << std::endl;
        else if (cmd == "NICK")
            handleNick(client, tokens);
        else if (cmd == "USER")
            handleUser(client, tokens);
        else if (cmd == "PASS")
            handlePass(client, tokens);

        if (!client.getNickname().empty() && !client.getUsername().empty())
        {
            (void)pass;
            client.authenticate();
            std::string nick = client.getNickname();
            std::cout << nick + " is auth." << std::endl;
            sendToClient(client.getFd(), ":ft_irc 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!\r\n");
            sendToClient(client.getFd(), ":ft_irc 002 " + nick + " :Your host is ft_irc, running version 1.0\r\n");
            sendToClient(client.getFd(), ":ft_irc 003 " + nick + " :This server was created today\r\n");
        }
    }
    else
    {
        if (cmd == "WHOIS")
            std::cout << "don't know" << std::endl;
        else if (cmd == "PASS")
            handlePass(client, tokens);
        else if (cmd == "NICK")
            handleNick(client, tokens);
        else if (cmd == "USER")
            handleUser(client, tokens);
        else if (cmd == "JOIN")
            handleJoin(client, channels, tokens);
        else if (cmd == "PRIVMSG")
            handlePrivmsg(client, channels, tokens);
        else if (cmd == "QUIT")
            std::cout
                << "leaving" << std::endl; // segv if client leave cause of no handling}
    }
}
void Command::handlePass(Client &client, const std::vector<std::string> &args)
{
    // change the password of a client
    (void)client;
    (void)args;
    std::cout << "handler called" << std::endl;
}

void Command::handleNick(Client &client, const std::vector<std::string> &args)
{
    // change the nickname of a client
    client.setNickname(args[1]);
    std::cout << "new NICK " + client.getNickname() << std::endl;
}

void Command::handleUser(Client &client, const std::vector<std::string> &args)
{
    // change the username of a client
    client.setUsername(args[1]);
    std::cout << "new USER " + client.getUsername() << std::endl;
}

void Command::handleJoin(Client &client, std::map<std::string, Channel *> channels, const std::vector<std::string> &args)
{
    // check if the channel exist if yes check the invites / join settings otherwise create a channel
    std::string channelName(args[1]);
    channels[channelName] = new Channel(channelName);
    channels[channelName]->addClient(&client);
    std::cout << client.getNickname() + " created: " + channels[channelName]->getChannelName() << std::endl;
}

void Command::handlePrivmsg(Client &client, std::map<std::string, Channel *> channels, const std::vector<std::string> &args)
{
    // used for any message between client -> channel / client -> client
    // PRIVMSG #channel :hello everyone!\r\n
    std::string channelName(args[1]);
    std::string msg(":ft_irc <HELLO>!<HELLO>@<localhost> PRIVMSG #hello :hello from the server!\r\n");
    if (channelName == "#hello")
        sendToClient(client.getFd(), msg);
        // channels[channelName]->forwardMessage(client.getFd(), msg);
    // send constructed message to the specific channel
    (void)channels;

}

// if a message is sent to a channel need to send it to all participant of that channel
// to get historique need to have std::vector<string> that got all previous message ready to send to a new joining client
// forward message format <nick>!<user>@<host> PRIVMSG #channel :hello everyone!\r\n