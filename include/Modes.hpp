#pragma once

#include <map>

enum ChannelModes
{
            MODE_I,
            MODE_T,
            MODE_K,
            MODE_O,
            MODE_L,
};

class Modes
{
    private:
        std::map<ChannelModes, bool>    _modeList;

    public:
        Modes();
        ~Modes();

        //Getters & Setters
        bool    getMode(ChannelModes mode) const;
        void    setMode(ChannelModes mode, bool b);
};