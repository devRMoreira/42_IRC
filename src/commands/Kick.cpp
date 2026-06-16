#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

//MISSING BETTER PARSING/REASON/: USAGE

void Server::handleKick(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);
	std::string channelName = arg.substr(0, arg.find(' '));
	std::string nickname = arg.substr(arg.find(' ') + 1);

	std::string reason = "reason";

	Channel * channel = NULL;
	Client * kicked = getClient(nickname);

	if (!kicked)
	{ // ERR_NOSUCHNICK (401)
		client.sendMessage(createReply(Reply::ERR_NOSUCHNICK, client.getNickname(), channelName));
		return ;
	}

	channel = getChannel(channelName);
	if (!channel)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL, client.getNickname(), channelName));
		return ;
	}
	else if (!channel->isMember(kicked) )
	{ // ERR_USERNOTINCHANNEL (441) 
		client.sendMessage(createReply(Reply::ERR_USERNOTINCHANNEL, client.getNickname(), nickname, channelName));
		return ;
	}
	else if (!channel->isOperator(&client) )
	{ // ERR_CHANOPRIVSNEEDED (482)
		client.sendMessage(createReply(Reply::ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName));
		return ;
	}

	//SUCESS
	channel->broadcast(client.getPrefix() +  " KICK " + channelName + " " + nickname + " :" + reason);
	channel->removeClient(*kicked);
}