#include "../include/Modes.hpp"

Modes::Modes()
{
    _modeList[MODE_I] = false;
    _modeList[MODE_T] = false;
    _modeList[MODE_K] = false;
    _modeList[MODE_O] = false;
    _modeList[MODE_L] = false;
}

Modes::~Modes()
{}

// Getters & Setters
bool    Modes::getMode(ChannelModes mode) const
{
    std::map<ChannelModes, bool>::const_iterator it = _modeList.find(mode);
    if (it != _modeList.end())
        return (it->second);
    return (false);
}

void    Modes::setMode(ChannelModes mode, bool b)
{ _modeList[mode] = b; }
