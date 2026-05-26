#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/constants.hpp"

#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <iostream>

static int initListener(const char* port);

Server::Server(std::string port, std::string pw)
	: _password(pw), _port(port)
{
	int res = initListener(port.c_str());

	if(res == ServerConstants::ERR_VAL)
	{
		throw("Error initializing server\n");
	}

	pollfd pfd;

	pfd.fd = res;
	pfd.events = POLLIN;

	_pfds.push_back(pfd);
	_listenerFd = res;
}

Server::~Server()
{
	std::map<int, Client*>::iterator it;

	for(it = _clients.begin(); it != _clients.end(); it++)
	{
		delete it->second;
	}
}


static int initListener(const char* port)
{
	int listenerFd;
	int yes = 1;
	int rv;

	addrinfo hints, *addrinfo, *p;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	rv = getaddrinfo(NULL, port, &hints, &addrinfo);

	if(rv != 0)
		return ServerConstants::ERR_VAL;

	for(p = addrinfo; p != NULL; p = p->ai_next)
	{
		listenerFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(listenerFd < 0)
			continue;

		setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if(bind(listenerFd, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listenerFd);
			continue;
		}

		break;
	}

	if(p == NULL)
		return ServerConstants::ERR_VAL;

	freeaddrinfo(addrinfo);

	if(listen(listenerFd, 10) == ServerConstants::ERR_VAL)
		return ServerConstants::ERR_VAL;

	return listenerFd;
}

static std::string convertToAddress(void *address, char *buffer, size_t size)
{
	sockaddr_storage *addressStorage = static_cast<sockaddr_storage*> (address);
	sockaddr_in *v4address;
	sockaddr_in6 *v6address;
	void *src;

	switch(addressStorage->ss_family)
	{
		case AF_INET:
			v4address = static_cast<sockaddr_in*> (address);
			src = &(v4address->sin_addr);
			break;
		case AF_INET6:
			v6address = static_cast<sockaddr_in6*> (address);
			src = &(v6address->sin6_addr);
			break;
		default:
			return std::string("Unknown address type");
	}

	inet_ntop(addressStorage->ss_family, src, buffer, size);
	return std::string(buffer);
}

void Server::removePfd(int fd)
{
    for (size_t i = 0; i < _pfds.size(); i++)
    {
        if (_pfds[i].fd == fd)
        {
            _pfds.erase(_pfds.begin() + i);
            return;
        }
    }
}


void Server::disconnectClient(int fd)
{
	std::string username = _clients[fd]->getUsername();

	//! remove from channels when implemented before deleting

	removePfd(fd);

	delete _clients[fd];
	_clients.erase(fd);


	std::cout << "Client " << username << " disconnected.\n";
}

void Server::connectClient(int fd)
{
	pollfd pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	_pfds.push_back(pfd);

	_clients[fd] = new Client(fd);
}

pollfd* Server::getListener()
{
	return &_pfds.at(0);
}

int Server::getFdCount() const
{
	return _pfds.size();
}

void Server::handleNewConnection()
{
	sockaddr_storage clientAddress;
	socklen_t addressLen;
	int newFd;
	char clientIP[INET6_ADDRSTRLEN];

	addressLen = sizeof clientAddress;

	newFd = accept(_listenerFd, (sockaddr *) &clientAddress, &addressLen);

	if(newFd == ServerConstants::ERR_VAL)
		std::cerr << "accept failed\n";
	else
	{
		connectClient(newFd);

		std::cout << "New connection from "
				  << convertToAddress(&clientAddress, clientIP, sizeof clientIP)
				  << " on socket " << newFd << "\n";
	}
}


void Server::handleClientData(int& index)
{
	char buffer[ServerConstants::BUFFER_SIZE];

	int senderFd = _pfds.at(index).fd;
	int bytesReceived = recv(senderFd, buffer, sizeof buffer, 0);


	if(bytesReceived <= 0)
	{
		close(senderFd);
		disconnectClient(senderFd);
		index--;
	}
	else
	{
		//* handle the data

		Client *client = _clients[senderFd];

		// std::cout << "Server::handleClientData()\n" << std::string(buffer, bytesReceived) << "end\n";

		client->addToBuffer(std::string(buffer, bytesReceived));

		std::vector<std::string> lines = client->getLines();

		std::cout << "\nParsed Lines\n";
		for(size_t i = 0; i < lines.size(); i++)
		{
			std::cout << "Line: "<< i + 1 << " - " << lines[i] << "\n";
		}
		std::cout << "end\n";
	}
}


void Server::run()
{
	for(int i = 0; i < getFdCount(); i++)
	{
		if(_pfds[i].revents & (POLLIN | POLLHUP))
		{
			if(_pfds[i].fd == _listenerFd)
				handleNewConnection();
			else
				handleClientData(i);

		}
	}
}
