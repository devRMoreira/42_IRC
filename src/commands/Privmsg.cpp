#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

void Server::handlePrivMsg(Client& sender, const std::string& line)
{
	std::string arg = extractArg(line);
	std::string targetName = arg.substr(0, arg.find(' '));
	std::string msg = arg.substr(arg.find(' ') + 1);

	if (targetName[0] == '#') 
	{
		Channel * channel = getChannel(targetName);
		if (channel)
			channel->broadcast(sender, msg);
		else // 403
			sender.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL, sender.getNickname(), targetName) );
		return ;
	}

	Client * targetUser = getClient(targetName);
	if (targetUser)
		targetUser->sendMessage(sender.getNickname() + "<prefix> PRIVMSG :" + msg + "\r\n"); // CHANGE
	else
		sender.sendMessage(createReply(Reply::ERR_NOSUCHNICK, sender.getNickname(), targetName) );
}
