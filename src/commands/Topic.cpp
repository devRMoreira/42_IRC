#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/message.hpp"

#include <string>


static bool isEmpty(const std::string& arg)
{
	return arg.size() == 1 && arg[0] == ':';
}

static std::string extractTopic(const std::vector<std::string>& args)
{
	std::string topic;

	for(size_t i = 1; i < args.size(); i++)
	{
		topic += args[i];
		if(i + 1 < args.size())
			topic += " ";
	}

	if(!topic.empty() && topic[0] == ':')
		topic.erase(topic.begin());

	return topic;
}

static void topicReply(const Channel& channel, const Client& client)
{
	if(channel.getTopic().empty())
		client.sendMessage(createReply(Reply::RPL_NOTOPIC, client.getNickname(), channel.getName()));
	else
		client.sendMessage(createReply(Reply::RPL_TOPIC, client.getNickname(), channel.getName(), channel.getTopic()));
}

#include <iostream>

static void changeTopic(Channel& channel, const Client& client, const std::vector<std::string>& args)
{
	std::string topic;

	std::cout << "HERE" << std::endl;
	for(size_t i = 0; i < args.size(); i++)
	{
		std::cout << i << " " << args[i] << std::endl;
	}

	if(args.size() == 2 && isEmpty(args[1]))
		topic = "";
	else
		topic = extractTopic(args);

	if(channel.isTopicProtected() && !channel.isOperator(&client))
		client.sendMessage(createReply(Reply::ERR_CHANOPRIVSNEEDED, client.getNickname(), channel.getName()));
	else
	{
		channel.setTopic(topic);

		std::string msg = client.getPrefix() +
						"TOPIC " + channel.getName() +
						" :" + topic + "\r\n";

		channel.broadcast(msg);
	}
}

void Server::handleTopic(Client& client, const std::string& line)
{
	std::vector<std::string> args = extractMultipleArgs(line);


	std::cout << "HERE" << std::endl;
	for(size_t i = 0; i < args.size(); i++)
	{
		std::cout << i << " " << args[i] << std::endl;
	}

	if(args.size() > 0)
	{
		std::string channelName = args[0];

		Channel * channel = getChannel(channelName);

		if(channel)
		{
			if(!channel->isMember(&client))
			{
				client.sendMessage(createReply(Reply::ERR_NOTONCHANNEL, client.getNickname(), channel->getName()));
			}
			else if(args.size() == 1)
			{
				topicReply(*channel, client);
			}
			else
			{
				changeTopic(*channel, client, args);
			}
		}
		else
		{
			client.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL, client.getNickname(), channelName));
		}
	}
	else
	{
		client.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS, client.getNickname(), "TOPIC"));
	}
}