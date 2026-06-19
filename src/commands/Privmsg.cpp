#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

#include <iostream>

void Server::handlePrivMsg(Client& sender, const std::string& line)
{
	std::vector<std::string> args = getArgsWithColon(line);

	if (args.size() < 2)
	{ // 461 ERR_NEEDMOREPARAMS
		sender.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS,
			sender.getNickname(), "PRIVMSG") );
		return ;
	}
	
	std::string targetName = args[0];
	std::string msg;

	if (args.back()[0] == ':')
		msg = args.back();
	else
		msg = ":" + args[1];

	if (targetName[0] == '#') 
	{
		Channel * channel = getChannel(targetName);
		if (channel)
        {
			channel->broadcast(sender, sender.getPrefix() + "PRIVMSG "
				+ targetName + " :" + msg + "\r\n");
        }
		else // 403 ERR_NOSUCHCHANNEL
		{
			sender.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL,
				sender.getNickname(), targetName) );
		}
		return ;
	}

	Client * targetUser = getClient(targetName);
	if (targetUser)
	{
		targetUser->sendMessage(sender.getPrefix() + "PRIVMSG "
			+ targetName + " :" + msg + "\r\n");
	}
	else // 401 ERR_NOSUCHNICK
	{
		sender.sendMessage(createReply(Reply::ERR_NOSUCHNICK,
			sender.getNickname(), targetName) );
	}
}
