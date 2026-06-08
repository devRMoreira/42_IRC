#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"

#include <algorithm>

Channel::Channel(const std::string& name) : _name(name)
{
}

void Channel::addOperator(Client& client)
{
	_members.push_back(Member(&client, true));
}

void Channel::addClient(Client& client)
{
	_members.push_back(Member(&client, false));
}

void Channel::removeClient(Client& client)
{
	std::vector<Member>::iterator it;
	for (it = _members.begin(); it != _members.end(); it++)
	{
		if (it->client == &client)
			break;
	}
	if (it != _members.end() )
		_members.erase(it);
}

void Channel::setTopic(const std::string& topic)
{
	_topic = topic;
}

void Channel::broadcast(Client& sender, const std::string& msg)
{
	for(size_t i = 0; i < _members.size(); i++)
	{
		if(_members[i].client != &sender)
			_members[i].client->sendMessageToClient(msg);
	}
}

bool Channel::isMember(const std::string& nick) const
{
	for(size_t i = 0; i < _members.size(); i++)
	{
		if(_members[i].client->getNickname() == nick)
			return true;
	}

	return false;
}

bool Channel::isOperator(const std::string& nick) const
{
	for(size_t i = 0; i < _members.size(); i++)
	{
		if(_members[i].client->getNickname() == nick)
			return _members[i].isOperator;
	}

	return false;
}

const std::string Channel::getName() const
{
	return(_name);
}