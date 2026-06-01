#include "../../inc/core/Channel.hpp"

Channel::Channel(const std::string& name) : _name(name)
{
}

void Channel::addOperator(Client& client)
{
	_operators.push_back(&client);
}

void Channel::addClient(Client& client)
{
	_clients.push_back(&client);
}


// void Channel::broadcast()
// {
// 	for(int j = 0; j < _pfds.size(); j++)
// 	{
// 		int destFd = _pfds[j].fd;

// 		if(destFd != _listenerFd && destFd != senderFd)
// 		{
// 			if(sendAll(destFd, buffer, bytesReceived) == -1)
// 			{
// 				std::cerr << "sendAll error\n";
// 				std::cout << "Only sent " << bytesReceived << " due to the error!\n";

// 			}
// 		}
// 	}
// }