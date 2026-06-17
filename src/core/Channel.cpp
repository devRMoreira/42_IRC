#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"

#include <iostream>
#include <algorithm>

Channel::Channel(const std::string& name) : _name(name), _topic(""), _key(""), _userLimit(0), _inviteOnly(false), _topicProtected(false)
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
	removeClientInvite(&client);
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

void Channel::setOperator(const Client* client, bool val)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i].client == client)
		{
			_members[i].isOperator = val;
			return;
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
			_members[i].client->sendMessage(msg);
	}
}

void Channel::broadcast(const std::string& msg)
{
	for(size_t i = 0; i < _members.size(); i++)
	{
		_members[i].client->sendMessage(msg);
	}
}

bool Channel::isMember(const Client* client) const
{
	for(size_t i = 0; i < _members.size(); i++)
	{
		if(_members[i].client == client)
			return true;
	}

	return false;
}

bool Channel::isOperator(const Client* client) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i].client == client)
			return _members[i].isOperator;
	}
	return false;
}

std::string Channel::getMemberNames() const
{
	std::string res;

	for(size_t i = 0; i < _members.size(); i++)
	{
		if(_members[i].isOperator)
			res += '@';

		res += _members[i].client->getNickname();

		if(i + 1 < _members.size())
			res += ' ';
	}

	return res;
}

std::string Channel::getModeString() const
{
	std::string flags;

	std::string params;

	if(_inviteOnly || _topicProtected || !_key.empty() || _userLimit != 0)
		flags += '+';

	if(_inviteOnly)
		flags += 'i';

	if(!_key.empty())
	{
		flags += 'k';
		params += "secret";
	}

	if(_userLimit != 0)
	{
		flags += 'l';

		if(!params.empty())
			params += " ";

		params += intToString(_userLimit);
	}

	if(_topicProtected)
		flags += 't';

	return flags + (params.empty() ? "" : " " + params);
}

void Channel::addClientInvite(const Client* client)
{
	_invitedClients.push_back(client);
}

void Channel::removeClientInvite(const Client* client)
{
	for(size_t i = 0; i < _invitedClients.size(); i++)
	{
		if(_invitedClients[i] == client)
		{
			_invitedClients.erase(_invitedClients.begin() + i);
			return;
		}
	}

}

bool Channel::isClientInvited(const Client* client)
{
	for(size_t i = 0; i < _invitedClients.size(); i++)
	{
		if(_invitedClients[i] == client)
			return true;
	}

	return false;
}

bool Channel::isKeyProtected() const
{
	return _key.empty();
}

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

bool Channel::hasUserLimit() const
{
	return _userLimit > 0;
}

unsigned int Channel::getUserLimit() const
{
	return _userLimit;
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

std::string Channel::getKey() const
{
	return _key;
}

