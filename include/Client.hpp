#pragma once

#include <string>
#include <map>

class Channel;

class Client
{
    private:
        int                                 _fd;
        bool                                _isAuth;
        std::string                         _nickname;
        std::string                         _username;
        std::map<std::string, Channel *>    _channelList;

    public:
        Client(int fd);
        ~Client();

        // Getters & Setters
        int             getFd() const;
        bool            isAuthenticated() const;
        std::string     getNickname() const;
        std::string     getUsername() const;

        void            setNickname(const std::string& nickname);
        void            setUsername(const std::string& username);

        // Methodes
        void            authenticate();
        void            joinChannel(Channel* channel);
        void            leaveChannel(Channel* channel);
        void            listChannels();
};
