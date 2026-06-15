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
	else if(cmd == "DEBUG")
	{
		std::cout << "pass set: " << client.isPassAccepted() << "\n";
		std::cout << "nick set: " << client.hasNick() << "\n";
		if (client.hasNick())
			std::cout << "nick: " << client.getNickname() << "\n";
		std::cout << "user set: " << client.hasUsername() << "\n";
		if (client.hasUsername())
		{
			std::cout << "user: " << client.getUsername() << "\n";
			std::cout << "real: " << client.getRealname() << "\n";
		}
		std::cout << "is registered?: " << client.isRegistered() << "\n";
	}
	//QUIT is the only other non-registered command
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
	else if(cmd == "JOIN")
		handleJoin(client, line);
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

void Server::handlePass(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);

	if (!client.isRegistered() )
	{
		if(arg == _password)
		{
			client.setPassAccepted();
			attemptRegistration(client);
		}
		else
		{
			// std::cout << "wrong! password is:\n'" << _password << "'\nyour input:\n'" << arg << "'\n";
			// passAccepted should be set to false
			// 464 ERR_PWDMISMATCH
			client.sendMessage(createReply(Reply::ERR_PASSWDMISMATCH, client));
			client.sendMessage(":ircserv 464 " + client.getNickname() + " :Password incorrect\r\n");
		}
	}
	else
	{
	//	client.sendMessage() 462 ERR_ALREADYREGISTERED
		client.sendMessage(":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
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
	if (!client.isPassAccepted() )
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
				client.sendMessage(":ircserv 433 "
					+ client.getNickname() + arg + " :Nickname is already in use\r\n");
			else
				// * in place of current NICK
				client.sendMessage(":ircserv 433 * "
					+ arg + " :Nickname is already in use\r\n");
			return ;
		}
	}
	//invalid NICK formats i.e. ? 432 ERR_ERRONEUSNICKNAME
	//
	client.setNickname(arg);

	if (!client.isRegistered()) //during registration, server silently accepts user’s request
	{
		attemptRegistration(client);
	}
	else //used after registration, server returns a NICK message
		client.sendMessage("<prefix> NICK :" + client.getNickname() + "\r\n");
}

void Server::handleUser(Client& client, const std::string& line) // needs more checks
{
	// ERROR :Password required before NICK/USER
	if (!client.isPassAccepted() )
	{
		client.sendMessage(":ircserv * :Password required before NICK/USER\r\n");
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
		attemptRegistration(client);
	}
	else
	{
	//	client.sendMessage() 462 ERR_ALREADYREGISTERED
		client.sendMessage(":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
	}
}

void Server::attemptRegistration(Client& client)
{
	if(client.isRegistered())
		return ;
	if(!client.canRegister())
		return ;

	client.setRegistered(true);

	client.sendMessage(createReply(Reply::RPL_WELCOME, client.getNickname(), client.getPrefix()));
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
		Channel * channel = getChannel(targetName);
		if (channel)
			channel->broadcast(sender, msg);
		else
			sender.sendMessage("<client> " + targetName + " :Cannot send to channel\r\n");
		return ;
	}

	Client * targetUser = getClient(targetName);
	if (targetUser)
		targetUser->sendMessage(sender.getNickname() + "<prefix> PRIVMSG :" + msg + "\r\n");
	else
	{	//ERR_WASNOSUCHNICK (406)
		sender.sendMessage("<client> " + targetName + " :There was no such nickname\r\n");
	}
}

void Server::handleInvite(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);
	std::string nickname = arg.substr(0, arg.find(' '));
	std::string channelName = arg.substr(arg.find(' ') + 1);

	Channel * channel = NULL;
	Client * invited = getClient(nickname);

	if (!invited)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessage(createReply(Reply::ERR_NOSUCHNICK, client.getNickname(), channelName));
		return ; 
	}

	channel = getChannel(channelName);
	if (!channel)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL, client.getNickname(), channelName));
		return ;
	}
	if (!channel->isMember(&client) )
	{ // ERR_NOTONCHANNEL (442)
		client.sendMessage(createReply(Reply::ERR_NOTONCHANNEL, client.getNickname(), channelName));
		return ;
	}
	else if (channel->isInviteOnly() && !channel->isOperator(invited) )
	{ // ERR_CHANOPRIVSNEEDED (482)
		client.sendMessage(createReply(Reply::ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName));
		return;
	}
	if (channel->isMember(invited) )
	{ // ERR_USERONCHANNEL (443)
		client.sendMessage(createReply(Reply::ERR_USERONCHANNEL, client.getNickname(), nickname, channelName));
		return ;
	}

	// SUCCESS
	clientJoinChannel(*invited, channel->getName());
	invited->sendMessage(client.getPrefix() + "INVITE " + nickname + " :" + channelName);
	client.sendMessage(createReply(Reply::RPL_INVITING, client.getNickname(), nickname, channelName));
}

void Server::handleKick(Client& client, const std::string& line)
{
	std::string arg = extractArg(line);
	std::string channelName = arg.substr(0, arg.find(' '));
	std::string nickname = arg.substr(arg.find(' ') + 1);

	//MISSING REASON
	std::string reason = "reason";

	Channel * channel = NULL;
	Client * kicked = getClient(nickname);

	if (!kicked)
	{ // ERR_NOSUCHNICK (401)
		client.sendMessage(createReply(Reply::ERR_NOSUCHNICK, client.getNickname(), channelName));
		return ;
	}

	channel = getChannel(channelName);
	if (!channel)
	{ // ERR_NOSUCHCHANNEL (403)
		client.sendMessage(createReply(Reply::ERR_NOSUCHCHANNEL, client.getNickname(), channelName));
		return ;
	}
	else if (!channel->isMember(kicked) )
	{ // ERR_USERNOTINCHANNEL (441) 
		client.sendMessage(createReply(Reply::ERR_USERNOTINCHANNEL, client.getNickname(), nickname, channelName));
		return ;
	}
	else if (!channel->isOperator(&client) )
	{ // ERR_CHANOPRIVSNEEDED (482)
		client.sendMessage(createReply(Reply::ERR_CHANOPRIVSNEEDED, client.getNickname(), channelName));
		return ;
	}

	//SUCESS
	channel->broadcast(client.getPrefix() +  " KICK " + channelName + " " + nickname + " :" + reason);
	channel->removeClient(*kicked);
}

Channel* Server::getChannel(const std::string& name)
{
    std::string key = normalizeString(name);

    std::map<std::string, Channel>::iterator it = _channels.find(key);
    if (it == _channels.end())
        return NULL;

    return &it->second;
}