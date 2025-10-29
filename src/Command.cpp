#include "../include/Command.hpp"

void Command::handleCommand(Client &client, std::map<std::string, Channel *> &channels, std::string &command, const std::string pass)
{
    std::vector<std::string> tokens = split(command, ' ');
    std::string cmd = tokens[0];

    if (cmd == "PING")
        sendToClient(client.getFd(), "PONG :ft_irc\r\n");
    if (client.isAuthenticated() == false)
    {
        // handle base setup before anything
        if (cmd == "WHOIS")
            std::cout << "don't know" << std::endl;
        else if (cmd == "NICK")
            handleNick(client, tokens);
        else if (cmd == "USER")
            handleUser(client, tokens);
        else if (cmd == "PASS")
            handlePass(client, tokens, pass);

        if (!client.getNickname().empty() && !client.getUsername().empty())
        {
            client.authenticate(); // need to be removed when isRegister is here
            std::string nick = client.getNickname();
            sendToClient(client.getFd(), ":ft_irc 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!\r\n");
            sendToClient(client.getFd(), ":ft_irc 002 " + nick + " :Your host is ft_irc, running version 1.0\r\n");
            sendToClient(client.getFd(), ":ft_irc 003 " + nick + " :This server was created today\r\n");
            handleCommand(client, channels, command, pass);
        }
    }
    else
    {
        if (cmd == "PASS")
            handlePass(client, tokens, pass);
        else if (cmd == "NICK")
            handleNick(client, tokens);
        else if (cmd == "USER")
            handleUser(client, tokens);
        else if (cmd == "JOIN")
            handleJoin(client, channels, tokens);
        else if (cmd == "PRIVMSG")
            handlePrivmsg(client, channels, tokens);
        else if (cmd == "QUIT")
            std::cout << "leaving" << std::endl; // segv if client leave cause of no handling}
    }
}
void Command::handlePass(Client &client, const std::vector<std::string> &args, const std::string pass)
{
    std::cout << "pass handler" << std::endl;
    if (args[1] == pass)
    {
        // client.authenticate(); // need isRegister to work otherwise doesn't work
        std::cout << client.getNickname() + " is auth" << std::endl;
    }
    else
        std::cout << "password is wrong" << std::endl;
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

void Command::handleJoin(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
    // check if the channel exist if yes check the invites / join settings otherwise create a channel
    std::string channelName(args[1]);
    if (!channels[channelName])
    {
        channels[channelName] = new Channel(channelName);
        std::cout << client.getNickname() + " created: " + channels[channelName]->getChannelName() << std::endl;
    }
    channels[channelName]->addClient(&client);
    std::cout << client.getNickname() + " joined: " + channels[channelName]->getChannelName() << std::endl;
}

void Command::handlePrivmsg(Client &client, std::map<std::string, Channel *> &channels, const std::vector<std::string> &args)
{
    if (args.size() < 3) // Need at least: PRIVMSG <target> :<message>
        return;

    std::string target = args[1]; // channel name

    // get the messsage after ':'
    std::string message = "";
    for (size_t i = 2; i < args.size(); i++)
    {
        if (i > 2)
            message += " ";
        message += args[i];
    }
    // Remove ':'
    if (!message.empty() && message[0] == ':')
        message = message.substr(1);

    // Build IRC message
    std::string formattedMsg = ":" + client.getNickname() + "!" +
                               client.getUsername() + "@localhost PRIVMSG " +
                               target + " :" + message + "\r\n";

    // Send to channel (if it exists)
    if (channels.find(target) != channels.end())
        channels[target]->sendMessageToClients(client.getFd(), formattedMsg);
}

// if a message is sent to a channel need to send it to all participant of that channel
// to get historique need to have std::vector<string> that got all previous message ready to send to a new joining client
// forward message format <nick>!<user>@<host> PRIVMSG #channel :hello everyone!\r\n

// to kick command KICK <channel> <user> [:<reason>] check op right from the user.fd