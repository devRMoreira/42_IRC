#include "../inc/core/Server.hpp"
#include "../inc/core/Client.hpp"

#include <sstream>
#include <iostream>

const std::string getErrorMessage(int errCode, unsigned int * paramCount);

void sendErrorMessage(int errCode, Client& client, std::vector<std::string> params)
{
    unsigned int paramCount = 0;
    std::string errMsg = getErrorMessage(errCode, &paramCount);

    if(params.size() != paramCount)
    {
        std::cerr << "wrong number of parameters given\n";
        return ;
    }

    std::stringstream line;
    line << ":ircserv " << errCode << " " << client.getNickname();

    for(size_t i = 0; i < paramCount; i++)
        line << " " + params[i];

    line << " :" + errMsg;

    client.sendMessageToClient(line.str());
}

const std::string getErrorMessage(int errCode, unsigned int * paramCount)
{
    std::string message;

    switch (errCode)
    {
        case 401: // ERR_NOSUCHNICK / params: <nick>
            message = "No such nick/channel";
            *paramCount = 1;
            break;
        case 403: // ERR_NOSUCHCHANNEL / params: <channel>
            message = "No such channel";
            *paramCount = 1;
            break;
        case 404: // ERR_CANNOTSENDTOCHAN / params: <channel>
            message = "Cannot send to channel";
            *paramCount = 1;
            break;
        case 407: // ERR_TOOMANYTARGETS / params: <target>
            message = "Duplicate recipients. No message delivered";
            *paramCount = 1;
            break;
        case 411: // ERR_NORECIPIENT / params: <command>
            message = "No recipient given";
            *paramCount = 1;
            break;
        case 412: // ERR_NOTEXTTOSEND / params:
            message = "No text to send";
            break;
        case 421: // ERR_UNKNOWNCOMMAND	/ params: <command>
            message = "Unknown command";
            *paramCount = 1;
            break;
        case 431: // ERR_NONICKNAMEGIVEN / params:
            message = "No nickname given";
            break;
        case 432: // ERR_ERRONEUSNICKNAME / params: <nick>
            message = "Erroneous nickname";
            *paramCount = 1;
            break;
        case 433: // ERR_NICKNAMEINUSE / params: <nick>
            message = "Nickname is already in use";
            *paramCount = 1;
            break;
        case 441: // ERR_USERNOTINCHANNEL / params: <nick> <channel>
            message = "They aren't on that channel";
            *paramCount = 2;
            break;
        case 442: // ERR_NOTONCHANNEL / params: <channel>
            message = "You're not on that channel";
            *paramCount = 1;
            break;
        case 443: // ERR_USERONCHANNEL / params: <nick> <channel>
            message = "is already on channel";
            *paramCount = 2;
            break;
        case 461: // ERR_NEEDMOREPARAMS / params: <command>
            message = "Not enough parameters";
            *paramCount = 1;
            break;
        case 462: // ERR_ALREADYREGISTRED / params:
            message = "You may not reregister";
            break;
        case 464: // ERR_PASSWDMISMATCH / params:
            message = "Password incorrect";
            break;
        case 471: // ERR_CHANNELISFULL / params: <channel>
            message = "Cannot join channel (+l)";
            *paramCount = 1;
            break;
        case 472: // ERR_UNKNOWNMODE / params: <char>
            message = "is unknown mode char to me";
            *paramCount = 1;
            break;
        case 473: // ERR_INVITEONLYCHAN / params: <channel>
            message = "Cannot join channel (+i)";
            *paramCount = 1;
            break;
        case 475: // ERR_BADCHANNELKEY / params: <channel>
            message = "Cannot join channel (+k)";
            *paramCount = 1;
            break;
        case 476: // ERR_BADCHANMASK / params: <channel>
            message = "Bad channel mask";
            *paramCount = 1;
            break;
        case 482: // ERR_CHANOPRIVSNEEDED / params: <channel>
            message = "You're not channel operator";
            *paramCount = 1;
            break;
        case 501: // ERR_UMODEUNKNOWNFLAG / params:
            message = "Unknown MODE flag";
            break;
        case 502: // ERR_USERSDONTMATCH / params:
            message = "Cannot change mode for other users";
            break;
        default:
            message = "Unknown error";
    }

    return message;
}