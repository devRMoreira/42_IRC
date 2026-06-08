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
		if((args.begin() + i + 1) != args.end())
			topic += " ";
	}

	topic.erase(topic.begin());

	return topic;
}

static void topicReply(const Channel& channel, const Client& client)
{
	if(channel.getTopic().empty())
		client.sendMessageToClient(createReply(Reply::RPL_NOTOPIC, client.getNickname(), channel.getName()));
	else
		client.sendMessageToClient(createReply(Reply::RPL_TOPIC, client.getNickname(), channel.getName(), channel.getTopic()));
}

static void changeTopic(Channel& channel, const Client& client, const std::vector<std::string>& args)
{
	std::string topic;
	if(args.size() == 2 && isEmpty(args[1]))
		topic = "";
	else
		extractTopic(args);

	if(channel.isTopicProtected() && !channel.isOperator(client.getNickname()))
		client.sendMessageToClient(createReply(Reply::ERR_CHANOPRIVSNEEDED, client.getNickname(), channel.getName()));
	else
	{
		channel.setTopic(topic);

		std::string msg =
						":" + client.getNickname() +
						"!" + client.getUsername() +
						"@ircserv TOPIC " + channel.getName() +
						" :" + topic + "\r\n";

		channel.broadcast(msg);
	}
}

void Server::handleTopic(Client& client, const std::string& line)
{
	std::vector<std::string> args = extractMultipleArgs(line);

	if(args.size() > 0)
	{
		std::string channelName = args[0];

		if(channelExists(channelName))
		{
			Channel& channel = _channels.at(channelName);

			if(!channel.isMember(client.getNickname()))
			{
				client.sendMessageToClient(createReply(Reply::ERR_USERNOTINCHANNEL, client.getNickname(), channel.getName()));
			}
			else if(args.size() == 1)
			{
				topicReply(channel, client);
			}
			else
			{
				changeTopic(channel, client, args);
			}
		}
		else
		{
			client.sendMessageToClient(createReply(Reply::ERR_NOSUCHCHANNEL, client.getNickname(), channelName));
		}
	}
}