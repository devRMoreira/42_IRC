#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"
#include "../../inc/irc.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <map>


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
	std::string username = _clients[fd]->getNickname();

	std::map<std::string, Channel>::iterator it;

	for(it = _channels.begin(); it != _channels.end();)
	{
		if(it->second.isMember(_clients[fd]))
		{
			it->second.removeClient(*_clients[fd]);

			if(it->second.isEmpty())
			{
				_channels.erase(it++);
				continue;
			}
		}
		++it;
	}

	removePfd(fd);

	delete _clients[fd];
	_clients.erase(fd);

	std::cout << "Client " << username << "| fd: " << fd << " disconnected.\n";
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
			handleLine(*client, lines[i]);
		}
		std::cout << "end\n";
	}
}

void Server::handleLine(Client& client, const std::string& line)
{
	std::string cmd = extractCmd(line);

	std::cout << cmd << " handleLine\n";

	if(cmd == "CAP")
		handleCap(client, line);
	else if(cmd == "PASS")
		handlePass(client, line);
	else if(cmd == "NICK")
		handleNick(client, line);
	else if(cmd == "USER")
		handleUser(client, line);

	else if(!client.isRegistered())
		client.sendMessage(createReply(Reply::ERR_NOTREGISTERED, client.getNickname()));

	else if(cmd == "JOIN")
		handleJoin(client, line);
	else if(cmd == "MODE")
		parseMode(client, line);
	else if(cmd == "TOPIC")
		handleTopic(client, line);
	else if(cmd == "PRIVMSG")
		handlePrivMsg(client, line);
	else if(cmd == "INVITE")
		handleInvite(client, line);
	else if(cmd == "KICK")
		handleKick(client, line);
}

void Server::handleCap(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);

	if(arg == "END")
	{
		client.setCapEnd();
	}
	else
	{
		client.sendMessage(":ircserv CAP * LS :\r\n");
	}
}

Client* Server::getClient(const std::string& nick)
{
	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->isRegistered() && areEqualCapitalized(nick, it->second->getNickname()) )
		{
			return it->second;
		}
	}
	return NULL;
}

Channel* Server::getChannel(const std::string& name)
{
    std::string key = normalizeString(name);

    std::map<std::string, Channel>::iterator it = _channels.find(key);
    if (it == _channels.end())
        return NULL;

    return &it->second;
}