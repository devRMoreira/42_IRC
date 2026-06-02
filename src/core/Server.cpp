#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/irc.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>


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
	else if(cmd == "JOIN")
		handleJoin(client, line);

	else if(cmd == "DEBUG")
	{
		std::cout << "nick set: " << client.getNickBool() << "\n";
		if (client.getNickBool())
			std::cout << "nick: " << client.getNickname() << "\n";
		std::cout << "user set: " << client.getUserBool() << "\n";
		if (client.getUserBool())
		{
			std::cout << "user: " << client.getUsername() << "\n";
			std::cout << "real: " << client.getRealname() << "\n";
		}
		std::cout << "is registered?: " << client.isRegistered() << "\n";
	}

	// might need another structure to handle specific, not-registered errors
	else if(client.isRegistered() )
	{
		if(cmd == "PRIVMSG")
			handlePrivMsg(client, line);
	}

	//!check for registration for other cmds
	//if(!client.registered)
}

void Server::handleCap(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);

	if(arg == "END")
	{
		std::cout << "END\n";
		client.setCapEnd();
	}
	else
	{
		client.sendMessageToClient(":ircserv CAP * LS :\r\n");
	}
}

void Server::handlePass(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);

	if (!client.isRegistered() )
	{
		if(arg == _password)
		{
			client.setPassAccepted();
			std::cout << "password accepted\n"; 
		}
		else
		{
			std::cout << "wrong! password is:\n'" << _password << "'\nyour input:\n'" << arg << "'\n"; 
			//	passAccepted should be set to false
			//	client.sendMessage() 464 ERR_PWDMISMATCH
		}
	}
	else
	{
	//	client.sendMessage() 462 ERR_ALREADYREGISTERED 
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
	// ERROR :Password required before NICK/USER

	std::string arg = extractArg(line);

	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		// after match is found, prints and returns
		if (it->second->isRegistered() && areEqualCapitalized(arg, it->second->getNickname()) ) 
		{
			//433 ERR_NICKNAMEINUSE - format "<client> <nick> :Nickname is already in use"c
			if (client.isRegistered())
				client.sendMessageToClient("<prefix> 433 "
					+ client.getNickname() + arg + " :Nickname is already in use\r\n");
			else
				// * in place of current NICK
				client.sendMessageToClient("<prefix> 433 * "
					+ arg + " :Nickname is already in use\r\n");
			return ;
		}
	}
	//invalid NICK formats i.e. ? 432 ERR_ERRONEUSNICKNAME
	//
	
	//during registration, server silently accepts user’s request
	client.setNickname(arg);

	//used after registration, server returns a NICK message
	if(client.isRegistered())
		client.sendMessageToClient("<prefix> NICK :" + client.getNickname() + "\r\n");

	if (!client.isRegistered())
	{	
		client.setNickBool();
		attemptRegistration(client);
	}
}

void Server::handleUser(Client& client, const std::string& line) // needs more checks
{
	// ERROR :Password required before NICK/USER

	std::string arg = extractArg(line);
	std::string username = arg.substr(0, arg.find(' '));
	std::string realname = arg.substr(arg.find(' ') + 1);

	client.setUsername(username);
	client.setRealname(realname);

	// std::cout << "client user: " << client.getUsername() << " real name : " << client.getRealname() << "\n";
	if (!client.isRegistered())
	{
		client.setUserBool();
		attemptRegistration(client);
	}
}

void Server::attemptRegistration(Client& client)
{
	if (client.getPassAccepted() && client.getNickBool() && client.getUserBool() )
	{
		client.setRegistration();
		//call function that prints successful registration messages
	}
}

Client* Server::nickExists(const std::string& nick)
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

void Server::handlePrivMsg(Client& sender, const std::string& line)
{
	std::string arg = extractArg(line);
	std::string nick = arg.substr(0, arg.find(' '));
	std::string msg = arg.substr(arg.find(' ') + 1);

	Client * target = nickExists(nick);
	if (target)
	{
		target->sendMessageToClient("<prefix> PRIVMSG :" + msg + "\r\n");
	}
	else
	{	//ERR_WASNOSUCHNICK (406)
		sender.sendMessageToClient("<client> " + nick + " :There was no such nickname\r\n");
	}
}