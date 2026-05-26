#include "../../inc/core/Channel.hpp"

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