#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

void Server::attemptRegistration(Client& client)
{
	if(client.isRegistered())
		return ;
	if(!client.canRegister())
		return ;

	client.setRegistered(true);

	client.sendMessage(createReply(Reply::RPL_WELCOME, client.getNickname(), client.getPrefix()));
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
		{ // 464 ERR_PWDMISMATCH
			client.sendMessage(createReply(Reply::ERR_PASSWDMISMATCH, ""));
		}
	}
	else
	{
	//	462 ERR_ALREADYREGISTERED
		client.sendMessage(createReply(Reply::ERR_ALREADYREGISTERED, client.getNickname()));
	}
}

void Server::handleNick(Client& client, const std::string& line)
{
	if (!client.isPassAccepted() )
	{
		client.sendMessage(":ircserv * :Password required before NICK/USER\r\n");
		return ;
	}

	std::string arg = extractArg(line);

	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->isRegistered() && areEqualCapitalized(arg, it->second->getNickname()) )
		{ // 433 ERR_NICKNAMEINUSE
			if (client.isRegistered())
				client.sendMessage(createReply(Reply::ERR_NICKNAMEINUSE, client.getNickname(), arg) );
			else
				client.sendMessage(createReply(Reply::ERR_NICKNAMEINUSE, "*", arg) );
			return ;
		}
	}
	if (arg[0] == '#' || arg[0] == ':')
	{ // 432 ERR_ERRONEUSNICKNAME
		client.sendMessage(createReply(Reply::ERR_ERRONEUSNICKNAME, client.getNickname(), arg) );
		return ;
	}
	
	client.setNickname(arg);

	if (!client.isRegistered())
	{
		attemptRegistration(client);
	}
	else
		client.sendMessage(":ircserv NICK :" + client.getNickname() + "\r\n");
}

void Server::handleUser(Client& client, const std::string& line) // needs more checks
{
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
	{ // 462 ERR_ALREADYREGISTERED
		client.sendMessage(createReply(Reply::ERR_ALREADYREGISTERED, client.getNickname()) );
	}
}