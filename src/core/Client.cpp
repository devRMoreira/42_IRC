#include "../../inc/core/Client.hpp"
#include "../../inc/constants.hpp"

#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

Client::Client(int fd) : _fd(fd), _passAccepted(false),  _nickIsSet(false), _userIsSet(false)
{
	(void) _registered;
}

void Client::addToBuffer(std::string data)
{
	_buffer += data;
}

std::vector<std::string> Client::getLines()
{
	//* parse every \r\n line

	std::vector<std::string> lines;

	size_t lineSize = _buffer.find("\r\n");

	while(lineSize != std::string::npos)
	{
		lines.push_back(_buffer.substr(0, lineSize));
		_buffer.erase(0, lineSize + 2);
		lineSize = _buffer.find("\r\n");
	}

	return lines;
}

static int sendData(int fd, char *buffer, int *len)
{
	int bytesTotal = 0;
	int bytesLeft = *len;
	int res;

	while (bytesTotal < bytesLeft)
	{
		res = send(fd, buffer + bytesTotal, bytesLeft, 0);
		if (res == -1)
			break;
		bytesTotal += res;
		bytesLeft -= res;
	}

	*len = bytesTotal;
	if (res == -1)
		return -1;
	else
		return 0;
}

int Client::sendMessageToClient(const std::string& msg)
{
	int	len = msg.size();
	char buffer[len + 1];

	for(int i = 0; i < len; i++)
		buffer[i] = msg[i];
	buffer[len] = '\0';

	if (sendData(_fd, buffer, &len) == ServerConstants::ERR_VAL)
	{
		std::cerr << "sendData error\n";
		std::cout << "Sent " << len << "/" << msg.size() << " bytes due to error!\n";
		return ServerConstants::ERR_VAL;
	}

	return 0;
}

void Client::setPassAccepted()
{
	_passAccepted = true;
}

bool Client::getPassAccepted() const
{
	return _passAccepted;
}

void Client::setUserBool()
{
	_userIsSet = true;
}

bool Client::getUserBool() const
{
	return _userIsSet;
}

void Client::setNickBool()
{
	_nickIsSet = true;
}

bool Client::getNickBool() const
{
	return _nickIsSet;
}


void Client::setNickname(const std::string& nickname)
{
	_nickname = nickname;
}

std::string Client::getNickname() const
{
	return _nickname;
}

void Client::setUsername(const std::string& username)
{
	_username = username;
}

std::string Client::getUsername() const
{
	return _username;
}

void Client::setRealname(const std::string& username)
{
	_realname = username;
}

std::string Client::getRealname() const
{
	return _realname;
}


bool Client::getCapEnd() const
{
	return _capEnd;
}

void Client::setCapEnd()
{
	_capEnd = true;
}

bool Client::isRegistered() const
{
	return _registered;
}
