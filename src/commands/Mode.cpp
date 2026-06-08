#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/irc.hpp"

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

void static handleMode(Client& client, Channel& channel, std::string modeString, std::vector<std::string> params)
{
	if(modeString.empty())
		return ;

	bool adding = modeString[0] == ModeFlag::ADD;
	size_t paramIndex = 0;

	for(size_t i = 1; i < modeString.size(); i++)
	{
		char c = modeString[i];

		if(c == ModeFlag::ADD)
			adding = true;
		else if(c == ModeFlag::REMOVE)
			adding = false;
		else if(c == ModeFlag::INVITE)
			channel.setInviteOnly(adding);
		else if(c == ModeFlag::TOPIC)
			channel.setTopicProtected(adding);
		else if(c == ModeFlag::KEY)
		{
			if(!adding)
				channel.setKey("");
			else if(adding && paramIndex < params.size())
				channel.setKey(params[paramIndex++]);
			else
				client.sendMessageToClient(":ircserv 461 " + client.getNickname() + " MODE :Not enough parameters");
		}
		else if(c == ModeFlag::OPERATOR)
		{
			if(paramIndex < params.size())
			{
				std::string nick = params[paramIndex++];

				if(channel.isMember(nick))
					channel.setOperator(nick, adding);
				else
					client.sendMessageToClient(":ircserv 441 " + client.getNickname() + nick + channel.getName() + " :User not in channel");
			}
			else
				client.sendMessageToClient(":ircserv 461 " + client.getNickname() + " MODE :Not enough parameters");

		}
		else if(c == ModeFlag::USER_LIMIT)
		{
			if(!adding)
				channel.setUserLimit(0);
			else if(paramIndex < params.size())
			{
				std::string param = params[paramIndex++];
				if(isNumeric(param))
				{
					long limit = strtol(param.c_str(), NULL, 10);

					if(limit <= ChannelConstants::MAX_CLIENTS)
					{
						channel.setUserLimit(static_cast<unsigned int>(limit));
					}
				}
			}
			else
				client.sendMessageToClient(":ircserv 461 " + client.getNickname() + " MODE :Not enough parameters");
		}
		else
			client.sendMessageToClient(":ircserv 501 " + client.getNickname() + " :Unknown MODE flag");
	}

	channel.debugChannel();
}

void Server::parseMode(Client& client, const std::string& line)
{
	std::vector<std::string> args = extractMultipleArgs(line);

	if(args.size() > 0)
	{
		std::string channelName = args[0];

		if(channelExists(args[0]) && args.size() > 1)
		{
			Channel& channel = _channels.at(channelName);

			if(channel.isOperator(client.getNickname()))
			{
				size_t i = 1;

				std::string modeString;
				std::vector<std::string> params;

				while(i < args.size() && (args[i][0] == ModeFlag::ADD || args[i][0] == ModeFlag::REMOVE))
				{
					modeString+=args[i];
					i++;
				}

				while(i < args.size())
				{
					params.push_back(args[i]);
					i++;
				}

				handleMode(client, channel, modeString, params);
			}
			else
				client.sendMessageToClient(":ircserv 482 " + client.getNickname() + " " + channelName + " :You're not channel operator");
		}
		else
		{
			client.sendMessageToClient(":ircserv modes");
		}
	}
}