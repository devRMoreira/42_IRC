#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include <iostream>

Channel::Channel(const std::string& name) : _name(name)
{
}

void Channel::debugChannel() const
{
	std::cout << "Members" << std::endl;
	for(size_t i = 0; i < _members.size(); i++)
	{
		std::cout << i << " Nick - " << _members[i].client->getNickname()
					   << " | Operator " << (_members[i].isOperator == true) << std::endl;
	}

	std::cout << "Name : " << _name << std::endl;
	std::cout << "Topic : " << _topic << std::endl;
	std::cout << "Topic protected : " << (_topicProtected == true) << std::endl;
	std::cout << "Key : " << _key << std::endl;
	std::cout << "User limit : " << _userLimit << std::endl;
	std::cout << "Invite only : " << (_inviteOnly == true) << std::endl;
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

void Channel::broadcast(const std::string& msg)
{
	for(size_t i = 0; i < _members.size(); i++)
	{
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

bool Channel::isTopicProtected() const
{
	return _topicProtected;
}

std::string Channel::getTopic() const
{
	return _topic;
}

