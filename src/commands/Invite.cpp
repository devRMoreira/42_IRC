#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

void Server::handleInvite(Client& client, const std::string& line)
{
	std::vector<std::string> args = getArgs(line);

	if (args.size() < 2)
	{ // 461 ERR_NEEDMOREPARAMS
		client.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS,
			client.getNickname(), "INVITE") );
		return ;
	}
	
	std::string nickname = args[0];
	std::string channelName = args[1];

	Client * invited = getClient(nickname);
	
	if (!invited)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessage(createReply(Reply::ERR_NOSUCHNICK,
			client.getNickname(), channelName));
		return ; 
	}

	Channel * channel = getChannel(channelName);

	if (!channel)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL,
			client.getNickname(), channelName));
		return ;
	}
	if (!channel->isMember(&client) )
	{ // ERR_NOTONCHANNEL (442)
		client.sendMessage(createReply(Reply::ERR_NOTONCHANNEL,
			client.getNickname(), channelName));
		return ;
	}
	else if (channel->isInviteOnly() && !channel->isOperator(invited) )
	{ // ERR_CHANOPRIVSNEEDED (482)
		client.sendMessage(createReply(Reply::ERR_CHANOPRIVSNEEDED,
			client.getNickname(), channelName));
		return;
	}
	if (channel->isMember(invited) )
	{ // ERR_USERONCHANNEL (443)
		client.sendMessage(createReply(Reply::ERR_USERONCHANNEL,
			client.getNickname(), nickname, channelName));
		return ;
	}

	// SUCCESS
	channel->addClientInvite(invited);
	invited->sendMessage(client.getPrefix() + "INVITE " + nickname + " :"
		+ channelName + "\r\n");
	client.sendMessage(createReply(Reply::RPL_INVITING, client.getNickname(),
		nickname, channelName));
}