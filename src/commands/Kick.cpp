#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

void Server::handleKick(Client& client, const std::string& line)
{
	std::vector<std::string> args = getArgsWithColon(line);

	if (args.size() < 2)
	{ // 461 ERR_NEEDMOREPARAMS
		client.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS,
			client.getNickname(), "KICK") );
		return ;
	}
	
	std::string reason = ""; // optional parameter

	std::string channelName = args[0];
	std::string nickname = args[1];
	if (args.size() >= 3)
	{
		if (args.back()[0] == ':')
			reason = args.back();
		else
			reason = ":" + args[2];
	}	

	Channel * channel = NULL;
	Client * nickToKick = getClient(nickname);

	if (!nickToKick)
	{ // ERR_NOSUCHNICK (401)
		client.sendMessage(createReply(Reply::ERR_NOSUCHNICK,
			client.getNickname(), channelName));
		return ;
	}

	channel = getChannel(channelName);
	if (!channel)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL,
			client.getNickname(), channelName));
		return ;
	}
	else if (!channel->isMember(nickToKick) )
	{ // ERR_USERNOTINCHANNEL (441) 
		client.sendMessage(createReply(Reply::ERR_USERNOTINCHANNEL,
			client.getNickname(), nickname, channelName));
		return ;
	}
	else if (!channel->isOperator(&client) )
	{ // ERR_CHANOPRIVSNEEDED (482)
		client.sendMessage(createReply(Reply::ERR_CHANOPRIVSNEEDED,
			client.getNickname(), channelName));
		return ;
	}

	// SUCCESS
	channel->broadcast(client.getPrefix() +  " KICK " + channelName + " "
		+ nickname + " " + reason + "\r\n");
	channel->removeClient(*nickToKick);
}