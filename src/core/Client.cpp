#include "../../inc/core/Client.hpp"

#include <vector>


Client::Client(int fd) : _fd(fd), _authenticated(false)
{
	(void)_fd;
	(void)_authenticated;
}

void Client::addToBuffer(std::string data)
{
	_buffer += data;
}

std::vector<std::string> Client::getLines() const
{
	std::vector<std::string> lines;

	//* parse every \r\n line

	return lines;
}

std::string Client::getUsername() const
{
	return _username;
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