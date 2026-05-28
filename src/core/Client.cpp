#include "../../inc/core/Client.hpp"

#include <vector>
#include <string>
#include <iterator>
#include <iostream>

Client::Client(int fd) : _fd(fd), _passAccepted(false)
{
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

std::string Client::getUsername() const
{
	return _username;
}

void Client::setPassAccepted()
{
	_passAccepted = true;
}

void Client::setUsername(const std::string& username)
{
	_username = username;
}

// static int sendMessage(int destFd, char *buffer, int& size)
// {
// 	int totalSent = 0;
// 	int leftToSend = size;
// 	int res;

// 	while(totalSent < size)
// 	{
// 		res = send(destFd, buffer+totalSent, size, 0);
// 		if(res == -1)
// 			break;
// 		totalSent += res;
// 		leftToSend -= res;
// 	}

// 	size = res;

// 	if(res == -1)
// 		return -1;
// 	else
// 		return 0;
// }