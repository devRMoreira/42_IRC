#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"
#include "../../inc/irc.hpp"

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

static std::string modeMsg(const Client& client, const Channel& channel, bool adding, char c, const std::string& param = "")
{
	std::string msg = client.getPrefix() + "MODE " + channel.getName() + " ";
	msg += (adding ? "+" : "-");
	msg += c;

	if(!param.empty())
		msg += " " + param;

	msg += "\r\n";
	return msg;
}

void Server::handleMode(Client& client, Channel& channel, std::string modeString, std::vector<std::string> params)
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
		{
			if(channel.setInviteOnly(adding))
				channel.broadcast(modeMsg(client, channel, adding, c));
		}
		else if(c == ModeFlag::TOPIC)
		{
			if(channel.setTopicProtected(adding))
				channel.broadcast(modeMsg(client, channel, adding, c));
		}
		else if(c == ModeFlag::KEY)
		{
			std::string key;
			if(!adding)
				key = "";
			else if(paramIndex < params.size())
				key = params[paramIndex++];
			else
			{
				client.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS, client.getNickname()));
				continue;
			}
			channel.setKey(key);
			channel.broadcast(modeMsg(client, channel, adding, c, key));

		}
		else if(c == ModeFlag::OPERATOR)
		{
			if(paramIndex < params.size())
			{
				std::string nick = params[paramIndex++];
				Client * target = getClient(nick);

				if(target && channel.isMember(target))
				{
					channel.setOperator(target, adding);
					channel.broadcast(modeMsg(client, channel, adding, c, target->getNickname()));
				}

				else
					client.sendMessage(createReply(Reply::ERR_USERNOTINCHANNEL, client.getNickname(), nick, channel.getName()));
			}
			else
				client.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS, client.getNickname()));

		}
		else if(c == ModeFlag::USER_LIMIT)
		{
			if(!adding)
			{
				channel.setUserLimit(0);
				channel.broadcast(modeMsg(client, channel, adding, c));
			}
			else if(paramIndex < params.size())
			{
				std::string param = params[paramIndex++];
				if(isNumeric(param))
				{
					long limit = strtol(param.c_str(), NULL, 10);

					if(limit <= ChannelConstants::MAX_CLIENTS && limit > 0)
					{
						channel.setUserLimit(static_cast<int>(limit));
						channel.broadcast(modeMsg(client, channel, adding, c, intToString(static_cast<int>(limit))));
					}
				}
			}
			else
				client.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS, client.getNickname()));
		}
		else
			client.sendMessage(createReply(Reply::ERR_UMODEUNKNOWNFLAG, client.getNickname()));
	}

	channel.debugChannel();
}

void Server::parseMode(Client& client, const std::string& line)
{
	std::vector<std::string> args = extractMultipleArgs(line);

	if(args.size() > 0)
	{
		std::string channelName = args[0];

		Channel* channel = getChannel(channelName);

		if(channel && args.size() > 1)
		{
			if(channel->isOperator(&client))
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

				handleMode(client, *channel, modeString, params);
			}
			else
				client.sendMessage(createReply(Reply::ERR_CHANOPRIVSNEEDED, client.getNickname(), channel->getName()));
		}
		else if(!channel)
			client.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL, client.getNickname(), channelName));
		else
		{
			std::cout << createReply(Reply::RPL_CHANNELMODEIS, client.getNickname(), channelName, channel->getModeString()) ;
			client.sendMessage(createReply(Reply::RPL_CHANNELMODEIS, client.getNickname(), channelName, channel->getModeString()));
		}

	}
}