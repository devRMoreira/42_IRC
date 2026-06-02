#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"

Channel::Channel(const std::string& name) : _name(name)
{
}

void Channel::addOperator(Client& client)
{
	_clients.push_back(Member(&client, true));
}

void Channel::addClient(Client& client)
{
	_clients.push_back(Member(&client, false));
}

void Channel::setTopic(const std::string& topic)
{
	_topic = topic;
}

void Channel::broadcast(Client& sender, const std::string& msg)
{
	for(size_t i = 0; i < _clients.size(); i++)
	{
		if(_clients[i].client != &sender)
			_clients[i].client->sendMessageToClient(msg);
	}
}

const std::string Channel::getName() const
{
	return(_name);
}