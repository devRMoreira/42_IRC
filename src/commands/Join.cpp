#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

#include <string>
#include <cctype>
#include <utility>

static bool isValidChannelName(const std::string& arg)
{
	if(arg.empty())
		return false;

	if(arg[0] != '#')
		return false;

	for(size_t i = 0; i < arg.size(); i++)
		if(arg[i] == ',')
			return false;

	if(arg.length() > ChannelConstants::MAX_NAME_SIZE)
		return false;

	return true;
}

#include <iterator>
#include <map>
#include <iostream>

static void printChannels(std::map<std::string, Channel> channels)
{
	std::cout << "Channel list\n" << std::endl;
	int counter = 1;

	for(std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		std::cout << counter++ <<" - "<< it->first << std::endl;
	}
}

void Server::clientJoinChannel(Client&  client, const std::string& arg)
{
	_channels.at(normalizeString(arg)).addClient(client);
}

void Server::createNewChannel(Client&  client, const std::string& arg)
{
	std::string normalized = normalizeString(arg);

	_channels.insert(std::make_pair(normalized, Channel(arg)));
	_channels.at(normalized).addOperator(client);
}

void Server::handleJoin(Client& client, const std::string& line)
{
	std::vector<std::string> args = extractMultipleArgs(line);

	if(args.empty())
		return;

	std::vector<std::string> targets = splitString(args[0], ',');
	std::vector<std::string> keys;

	if(args.size() > 1)
		keys = splitString(args[1], ',');

	for(size_t i = 0; i < targets.size(); i++)
	{
		std::string channelName = targets[i];
		std::string key = "";

		if(i < keys.size())
			key = keys[i];


		if(!isValidChannelName(channelName))
		{
			client.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL, client.getNickname(), channelName));
			continue;
		}

		Channel* channel;

		if(!channelExists(channelName))
		{
			createNewChannel(client, channelName);
			channel = getChannel(channelName);
		}
		else
		{
			channel = getChannel(channelName);

			if(channel->isMember(&client))
				continue;

			if(channel->isKeyProtected())
			{
				if(key != channel->getKey())
				{
					client.sendMessage(createReply(Reply::ERR_BADCHANNELKEY, client.getNickname(), channelName));
					continue;
				}
			}

			clientJoinChannel(client, channelName);
		}

		printChannels(_channels);

		channel->broadcast(client.getPrefix() + "JOIN :" + channelName);

		client.sendMessage(createReply(Reply::RPL_NAMREPLY, client.getNickname(), channelName, channel->getMemberNames()));

		client.sendMessage(createReply(Reply::RPL_ENDOFNAMES, client.getNickname(), channelName));
	}
}
