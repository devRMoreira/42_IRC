#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

#include <iostream>

//MISSING BETTER PARSING/ : USAGE

void Server::handlePrivMsg(Client& sender, const std::string& line)
{
	std::string arg = extractArg(line);
	std::string targetName = arg.substr(0, arg.find(' '));
	std::string msg = arg.substr(arg.find(' ') + 1);

	if (targetName[0] == '#') 
	{
		Channel * channel = getChannel(targetName);
		if (channel)
        {
            std::cout << "broadly casting\n";
			channel->broadcast(sender, sender.getPrefix() + "PRIVMSG " + targetName
            + " :" + msg + "\r\n");
        }
		else // 403
			sender.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL, sender.getNickname(), targetName) );
		return ;
	}

	Client * targetUser = getClient(targetName);
	if (targetUser)
		targetUser->sendMessage(sender.getPrefix() + "PRIVMSG " + targetName
            + " :" + msg + "\r\n");
	else
		sender.sendMessage(createReply(Reply::ERR_NOSUCHNICK, sender.getNickname(), targetName) );
}
