#pragma once

#include <string>
#include <map>

class Client
{
    private:
        int                             _fd;
        std::string                     _nickname;
        std::string                     _username;
        enum                            Role { OPERATOR, REGULAR };
        std::map<std::string, Role>     _channels;

    public:
        Client(int fd);
        ~Client();

        // Getters & Setters
        int             getFd();
        std::string     getNickname();
        std::string     getUsername();

        void            setNickname(std::string nickname);
        void            setUsername(std::string username);
};
