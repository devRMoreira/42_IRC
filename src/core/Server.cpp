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
	else if(cmd == "MODE")
		parseMode(client, line);
	else if(cmd == "TOPIC")
		handleTopic(client, line);

	else if(cmd == "DEBUG")
	{
		std::cout << "pass set: " << client.getPassAccepted() << "\n";
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
	//QUIT is the only other non-registered command

	else if(!client.isRegistered() )
	{	// ERR_NOTREGISTERED (451)
		client.sendMessageToClient(":ircserv 451 " + client.getNickname() + " :You have not registered\n");
		return ;
	}
	else if(cmd == "PRIVMSG")
		handlePrivMsg(client, line);
	else if(cmd == "JOIN")
		handleJoin(client, line);
	else if(cmd == "INVITE")
		handleInvite(client, line);
	else if(cmd == "KICK")
		handleKick(client, line);

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
			// std::cout << "password accepted\n"; 
		}
		else
		{
			// std::cout << "wrong! password is:\n'" << _password << "'\nyour input:\n'" << arg << "'\n"; 
			// passAccepted should be set to false
			// 464 ERR_PWDMISMATCH
			client.sendMessageToClient(":ircserv 464 " + client.getNickname() + " :Password incorrect\r\n");
		}
	}
	else
	{
	//	client.sendMessage() 462 ERR_ALREADYREGISTERED 
		client.sendMessageToClient(":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
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
	if (!client.getPassAccepted() )
	{
		// sendErrorMessage(0, client, std::vector<std::string>());
		return ;
	}

	std::string arg = extractArg(line);

	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->isRegistered() && areEqualCapitalized(arg, it->second->getNickname()) ) 
		{
			//433 ERR_NICKNAMEINUSE - format "<client> <nick> :Nickname is already in use"c
			if (client.isRegistered())
				client.sendMessageToClient(":ircserv 433 "
					+ client.getNickname() + arg + " :Nickname is already in use\r\n");
			else
				// * in place of current NICK
				client.sendMessageToClient(":ircserv 433 * "
					+ arg + " :Nickname is already in use\r\n");
			return ;
		}
	}
	//invalid NICK formats i.e. ? 432 ERR_ERRONEUSNICKNAME
	//	
	client.setNickname(arg);

	if (!client.isRegistered()) //during registration, server silently accepts user’s request
	{	
		client.setNickBool();
		attemptRegistration(client);
	}
	else //used after registration, server returns a NICK message
		client.sendMessageToClient("<prefix> NICK :" + client.getNickname() + "\r\n");
}

void Server::handleUser(Client& client, const std::string& line) // needs more checks
{
	// ERROR :Password required before NICK/USER
	if (!client.getPassAccepted() )
	{
		client.sendMessageToClient(":ircserv * :Password required before NICK/USER\r\n");
		return ;
	}

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
	else
	{
	//	client.sendMessage() 462 ERR_ALREADYREGISTERED 
		client.sendMessageToClient(":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
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

// Channel* Server::channelExists(const std::string& channel)
// {
// 	std::map<std::string, Channel>::iterator it;
// 	for (it = _channels.begin(); it != _channels.end(); it++)
// 	{
// 		if (areEqualCapitalized(channel, it->second.getName()) )
// 		{
// 			return &it->second;
// 		}
// 	}
// 	return NULL;
// }

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
	std::string targetName = arg.substr(0, arg.find(' '));
	std::string msg = arg.substr(arg.find(' ') + 1);

	//ADD general chanTypes string (like #&) for leading chars channel names, to distinguish from nicks
	// if (targetName[0] == '#' || targetName[0] == '&') // default values
	std::string chanTypes = "#&";

	if (chanTypes.find_first_of(targetName[0]) != std::string::npos) // if targetName's leading char is #/&
	{
		// Channel * targetChannel = channelExists(targetName);
		if (channelExists(targetName))
			_channels.at(targetName).broadcast(sender, msg);
		else
			sender.sendMessageToClient("<client> " + targetName + " :Cannot send to channel\r\n");
		return ;
	}

	Client * targetUser = nickExists(targetName);
	if (targetUser)
		targetUser->sendMessageToClient(sender.getNickname() + "<prefix> PRIVMSG :" + msg + "\r\n");
	else
	{	//ERR_WASNOSUCHNICK (406)
		sender.sendMessageToClient("<client> " + targetName + " :There was no such nickname\r\n");
	}
}

void Server::handleInvite(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);
	std::string nickname = arg.substr(0, arg.find(' '));
	std::string channelName = arg.substr(arg.find(' ') + 1);

	Channel * channel = NULL;
	Client * invited = nickExists(nickname);
	if (!invited)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessageToClient(":ircserv " + client.getNickname() + " " 
			+ channelName + " 403 :No such channel\r\n");
			return ;
	}
	channel = channelExists(channelName);
	if (!channel)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessageToClient(":ircserv 403 " + client.getNickname() + " " 
			+ channelName + " :No such channel\r\n");
		return ;
	}
	if (!channel->isMember(client.getNickname()) )
	{ // ERR_NOTONCHANNEL (442)
		client.sendMessageToClient(":ircserv " + client.getNickname() + " " 
			+ channelName + " 442 :You're not on that channel");
		return ;
	}
	// else if (!channel->isOperator(nickname) ) // && inviteOnly
	// { // ERR_CHANOPRIVSNEEDED (482)
	// 	client.sendMessageToClient(":ircserv " + client.getNickname() + " " 
	// 		+ channelName + " 482 :You're not channel operator");
	// }
	if (channel->isMember(nickname) )
	{ // ERR_USERONCHANNEL (443)
		client.sendMessageToClient(":ircserv " + client.getNickname() + " " 
			+ nickname + " " + channelName + " 443 :Is already on channel");
		return ;
	}

	// SUCCESS
	clientJoinChannel(*invited, channel->getName());
	invited->sendMessageToClient(client.getNickname() + " INVITE " + nickname + " :" + channelName + "\r\n");
	client.sendMessageToClient(":ircserv 341 " + client.getNickname()
		+ " " + (*invited).getNickname() + " " + channelName + "\r\n");
}

void Server::handleKick(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);
	std::string channelName = arg.substr(0, arg.find(' '));
	std::string nickname = arg.substr(arg.find(' ') + 1);
	
	Channel * channel = NULL;
	Client * kicked = nickExists(nickname);

	if (!kicked)
	{ // ERR_NOSUCHNICK (401)
		client.sendMessageToClient(":ircserv 401 " + client.getNickname() + " " 
			+ channelName + " :No such nick/channel\r\n");
		return ;
	}
	channel = channelExists(channelName);
	if (!channel)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessageToClient(":ircserv 403 " + client.getNickname() + " " 
			+ channelName + " :No such channel\r\n");
		return ;
	}
	else if (!channel->isMember(nickname) )
	{ // ERR_USERNOTINCHANNEL (441)
		client.sendMessageToClient(":ircserv 441 " + client.getNickname() + " " 
		+ nickname + " " + channelName + " :They aren't on that channel");
		return ;
	}
	// else if (!channel->isOperator(nickname) )
	// { // ERR_CHANOPRIVSNEEDED (482)
	// 	client.sendMessageToClient(":ircserv 482 " + client.getNickname()
	// 		+ channelName + " :You're not channel operator");
	// }

	//SUCESS
	channel->broadcast(client, ":" + client.getNickname() + " KICK " + channelName + " " + nickname + " :optional comment");
	client.sendMessageToClient(":" + client.getNickname() + " KICK " + channelName + " " + nickname + " :optional comment");
	channel->removeClient(*kicked); 
}

bool Server::channelExists(const std::string& str)
{
	for(std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if(it->first == str)
			return true;
	}

	return false;
}