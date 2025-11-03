#pragma once

#include <string>

// ============================================================================
// IRC NUMERIC REPLIES - RFC 1459 & RFC 2812
// ============================================================================

// ========================================================================
// WELCOME MESSAGES (001-005)
// ========================================================================
# define RPL_WELCOME           "001" // Welcome to the Internet Relay Network <nick>!<user>@<host>
# define RPL_YOURHOST          "002" // Your host is <servername>, running version <ver>
# define RPL_CREATED           "003" // This server was created <date>
# define RPL_MYINFO            "004" // <servername> <version> <available user modes> <available channel modes>
# define RPL_BOUNCE            "005" // Try server <server name>, port <port number>

// ========================================================================
// COMMAND RESPONSES (200-399)
// ========================================================================
# define RPL_AWAY              "301" // <nick> :<away message>
# define RPL_UNAWAY            "305" // :You are no longer marked as being away
# define RPL_NOWAWAY           "306" // :You have been marked as being away

# define RPL_WHOISUSER         "311" // <nick> <user> <host> * :<real name>
# define RPL_WHOISSERVER       "312" // <nick> <server> :<server info>
# define RPL_WHOISOPERATOR     "313" // <nick> :is an IRC operator
# define RPL_ENDOFWHO          "315" // <name> :End of WHO list
# define RPL_WHOISIDLE         "317" // <nick> <integer> :seconds idle
# define RPL_ENDOFWHOIS        "318" // <nick> :End of WHOIS list
# define RPL_WHOISCHANNELS     "319" // <nick> :*( ( "@" / "+" ) <channel> " " )

# define RPL_LISTSTART         "321" // Channel :Users  Name
# define RPL_LIST              "322" // <channel> <# visible> :<topic>
# define RPL_LISTEND           "323" // :End of LIST
# define RPL_CHANNELMODEIS     "324" // <channel> <mode> <mode params>
# define RPL_NOTOPIC           "331" // <channel> :No topic is set
# define RPL_TOPIC             "332" // <channel> :<topic>
# define RPL_TOPICWHOTIME      "333" // <channel> <nick> <setat>

# define RPL_INVITING          "341" // <channel> <nick>
# define RPL_INVITELIST        "346" // <channel> <invitemask>
# define RPL_ENDOFINVITELIST   "347" // <channel> :End of channel invite list
# define RPL_EXCEPTLIST        "348" // <channel> <exceptionmask>
# define RPL_ENDOFEXCEPTLIST   "349" // <channel> :End of channel exception list

# define RPL_NAMREPLY          "353" // ( "=" / "*" / "@" ) <channel> :[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )
# define RPL_ENDOFNAMES        "366" // <channel> :End of NAMES list
# define RPL_BANLIST           "367" // <channel> <banmask>
# define RPL_ENDOFBANLIST      "368" // <channel> :End of channel ban list

# define RPL_MOTDSTART         "375" // :- <server> Message of the day -
# define RPL_MOTD              "372" // :- <text>
# define RPL_ENDOFMOTD         "376" // :End of MOTD command

# define RPL_YOUREOPER         "381" // :You are now an IRC operator

// ========================================================================
// ERROR REPLIES (400-599)
// ========================================================================

// --- General Errors (401-424) ---
# define ERR_NOSUCHNICK        "401" // <nickname> :No such nick/channel
# define ERR_NOSUCHSERVER      "402" // <server name> :No such server
# define ERR_NOSUCHCHANNEL     "403" // <channel name> :No such channel
# define ERR_CANNOTSENDTOCHAN  "404" // <channel name> :Cannot send to channel
# define ERR_TOOMANYCHANNELS   "405" // <channel name> :You have joined too many channels
# define ERR_WASNOSUCHNICK     "406" // <nickname> :There was no such nickname
# define ERR_TOOMANYTARGETS    "407" // <target> :<error code> recipients. <abort message>
# define ERR_NOSUCHSERVICE     "408" // <service name> :No such service
# define ERR_NOORIGIN          "409" // :No origin specified

# define ERR_NORECIPIENT       "411" // :No recipient given (<command>)
# define ERR_NOTEXTTOSEND      "412" // :No text to send
# define ERR_NOTOPLEVEL        "413" // <mask> :No toplevel domain specified
# define ERR_WILDTOPLEVEL      "414" // <mask> :Wildcard in toplevel domain
# define ERR_BADMASK           "415" // <mask> :Bad Server/host mask

# define ERR_UNKNOWNCOMMAND    "421" // <command> :Unknown command
# define ERR_NOMOTD            "422" // :MOTD File is missing
# define ERR_NOADMININFO       "423" // <server> :No administrative info available
# define ERR_FILEERROR         "424" // :File error doing <file op> on <file>

// --- Nickname Errors (431-437) ---
# define ERR_NONICKNAMEGIVEN   "431" // :No nickname given
# define ERR_ERRONEUSNICKNAME  "432" // <nick> :Erroneous nickname
# define ERR_NICKNAMEINUSE     "433" // <nick> :Nickname is already in use
# define ERR_NICKCOLLISION     "436" // <nick> :Nickname collision KILL from <user>@<host>
# define ERR_UNAVAILRESOURCE   "437" // <nick/channel> :Nick/channel is temporarily unavailable

// --- User/Registration Errors (441-446) ---
# define ERR_USERNOTINCHANNEL  "441" // <nick> <channel> :They aren't on that channel
# define ERR_NOTONCHANNEL      "442" // <channel> :You're not on that channel
# define ERR_USERONCHANNEL     "443" // <user> <channel> :is already on channel
# define ERR_NOLOGIN           "444" // <user> :User not logged in
# define ERR_SUMMONDISABLED    "445" // :SUMMON has been disabled
# define ERR_USERSDISABLED     "446" // :USERS has been disabled

// --- Authentication Errors (451-467) ---
# define ERR_NOTREGISTERED     "451" // :You have not registered

# define ERR_NEEDMOREPARAMS    "461" // <command> :Not enough parameters
# define ERR_ALREADYREGISTRED  "462" // :Unauthorized command (already registered)
# define ERR_NOPERMFORHOST     "463" // :Your host isn't among the privileged
# define ERR_PASSWDMISMATCH    "464" // :Password incorrect
# define ERR_YOUREBANNEDCREEP  "465" // :You are banned from this server
# define ERR_YOUWILLBEBANNED   "466" // 
# define ERR_KEYSET            "467" // <channel> :Channel key already set

// --- Channel Errors (471-478) ---
# define ERR_CHANNELISFULL     "471" // <channel> :Cannot join channel (+l)
# define ERR_UNKNOWNMODE       "472" // <char> :is unknown mode char to me for <channel>
# define ERR_INVITEONLYCHAN    "473" // <channel> :Cannot join channel (+i)
# define ERR_BANNEDFROMCHAN    "474" // <channel> :Cannot join channel (+b)
# define ERR_BADCHANNELKEY     "475" // <channel> :Cannot join channel (+k)
# define ERR_BADCHANMASK       "476" // <channel> :Bad Channel Mask
# define ERR_NOCHANMODES       "477" // <channel> :Channel doesn't support modes
# define ERR_BANLISTFULL       "478" // <channel> <char> :Channel list is full

// --- Privilege Errors (481-485) ---
# define ERR_NOPRIVILEGES      "481" // :Permission Denied- You're not an IRC operator
# define ERR_CHANOPRIVSNEEDED  "482" // <channel> :You're not channel operator
# define ERR_CANTKILLSERVER    "483" // :You can't kill a server!
# define ERR_RESTRICTED        "484" // :Your connection is restricted!
# define ERR_UNIQOPPRIVSNEEDED "485" // :You're not the original channel operator

// --- Other Errors (491-502) ---
# define ERR_NOOPERHOST        "491" // :No O-lines for your host

# define ERR_UMODEUNKNOWNFLAG  "501" // :Unknown MODE flag
# define ERR_USERSDONTMATCH    "502" // :Cannot change mode for other users

// ========================================================================
// HELPER FUNCTIONS
// ========================================================================

std::string formatReply(const std::string &code, const std::string &target, const std::string &message);
std::string formatError(const std::string &code, const std::string &target, const std::string &param, const std::string &message);
