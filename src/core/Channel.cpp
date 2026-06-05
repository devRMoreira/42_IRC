#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"

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

void Channel::setOperator(const std::string& nick, bool val)
{
	for(size_t i = 0; i < _members.size(); i++)
	{
		if(_members[i].client->getNickname() == nick)
		{
			_members[i].isOperator = val;
			return ;
		}
	}
}

void Channel::setTopic(const std::string& str)
{
	_topic = str;
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

void Channel::setKey(const std::string& str)
{
	_key = str;
}
void Channel::setUserLimit(unsigned int n)
{
	if(n > 0)
		_userLimit = n;
}

void Channel::setInviteOnly(const bool val)
{
	_inviteOnly = val;
}
void Channel::setTopicProtected(const bool val)
{
	_topicProtected = val;
}

std::string Channel::getName() const
{
	return _name;
}