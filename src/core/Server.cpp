#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/irc.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>


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

		// std::cout << "\nParsed Lines\n";
		for(size_t i = 0; i < lines.size(); i++)
		{
			std::cout << "Line: "<< i + 1 << " - " << lines[i] << "\n";
			handleLine(*client, lines[i]);
		}
		// std::cout << "end\n";/
	}
}


void Server::handleLine(Client& client, const std::string& line)
{
	std::string cmd = extractCmd(line);

	std::cout << "cmd: '"<< cmd <<"'\n";

	if(cmd == "CAP" && !client.getCapEnd())
		handleCap(client, line);
	else if(cmd == "PASS" && !client.getPassAccepted())
		handlePass(client, line);
	else if(cmd == "NICK")
		handleNick(client, line);
	else if(cmd == "USER")
		handleUser(client, line);
	else if(cmd == "JOIN")
		handleJoin(client, line);


	//!check for registration for other cmds
	//if(!client.registered)
}

void Server::handleCap(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);

	std::cout << "arg: '"<< arg <<"'\n";


	if(arg == "END")
		client.setCapEnd();
	else
		client.sendMessageToClient(":ircserv CAP * LS :\r\n");
}

void Server::handlePass(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);


	std::cout << "arg: '"<< arg <<"'\n";

	if(arg == _password)
		client.setPassAccepted();
	else
	{
		client.sendMessageToClient(":ircserv " + client.getUsername() + " :Password incorrect\r\n");
	}
}

static bool areEqualCapitalized(const std::string& str1, const std::string& str2)
{
	if (str1.size() != str2.size())
		return false;
	else
	{
		for (size_t i = 0; i < str1.size(); i++)
		{
			if (toupper(str1[i]) != toupper(str2[i]))
				return(false);
		}
	}
	return true;
}

//how to handle whitespace at the edges? AKA 'bingus' vs 'bingus '
void Server::handleNick(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);
	bool available = true;

	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->isRegistered() && areEqualCapitalized(arg, it->second->getNickname()) ) // NO USERS ARE REGISTERED YET
		{
			available = false;
			//433 ERR_NICKNAMEINUSE - format "<client> <nick> :Nickname is already in use"c
			if (client.isRegistered())
				client.sendMessageToClient("<prefix> 433 "
					+ client.getNickname() + arg + " :Nickname is already in use\r\n");
			else
				// * in place of current NICK
				client.sendMessageToClient("<prefix> 433 * "
					+ arg + " :Nickname is already in use\r\n");
		}
	}
	if (available)
	{
		//invalid NICK formats i.e. ? 432 ERR_ERRONEUSNICKNAME
		//

		//during registration, server silently accepts user’s request
		client.setNickname(arg);

		//used after registration, server returns a NICK message
		if(client.isRegistered())
			client.sendMessageToClient("<prefix> NICK :" + client.getNickname() + "\r\n");

		// std::cout << "your new nick is " << client.getNickname() << "\n";
	}
}

void Server::handleUser(Client& client, const std::string& line) // needs more checks
{
	std::string arg = extractArg(line);
	std::string username = arg.substr(0, arg.find(' '));
	std::string realname = arg.substr(arg.find(' ') + 1);

	client.setUsername(username);
	client.setRealname(realname);

	// std::cout << "client user: " << client.getUsername() << " real name : " << client.getRealname() << "\n";
}
