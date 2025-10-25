#pragma

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Command
{
public:
    static void handleCommand(Client &client, Channel &channel, std::string &command);
    void handlePass(Client &client, const std::vector<std::string> &args);
    void handleNick(Client &client, const std::vector<std::string> &args);
    void handleUser(Client &client, const std::vector<std::string> &args);
    void handleJoin(Client &client, const std::vector<std::string> &args);
    void handlePrivmsg(Client &client, const std::vector<std::string> &args);

private:
};