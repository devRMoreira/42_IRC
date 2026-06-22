#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

#include <iostream>

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
	{ // 462 ERR_ALREADYREGISTERED
		client.sendMessage(createReply(Reply::ERR_ALREADYREGISTERED,
			client.getNickname()));
	}
}

void Server::handleNick(Client& client, const std::string& line)
{
	if (!client.isPassAccepted() )
	{
		client.sendMessage(":ircserv * :Password required before NICK/USER\r\n");
		return ;
	}

	std::vector<std::string> args = getArgs(line); 
	std::string nick = "";
	
	if (args.empty() )
	{
		if (client.isRegistered())
			client.sendMessage(createReply(Reply::ERR_NONICKNAMEGIVEN, "*") );
		else
			client.sendMessage(createReply(Reply::ERR_NONICKNAMEGIVEN,
				client.getNickname()) );
		return ;
	}
	
	nick = args[0];

	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->isRegistered()
			&& areEqualCapitalized(nick, it->second->getNickname()) )
		{ // 433 ERR_NICKNAMEINUSE
			if (client.isRegistered())
				client.sendMessage(createReply(Reply::ERR_NICKNAMEINUSE,
					client.getNickname(), nick) );
			else
				client.sendMessage(createReply(Reply::ERR_NICKNAMEINUSE,
					"*", nick) );
			return ;
		}
	}

	if (nick[0] == '#' || nick[0] == ':')
	{ // 432 ERR_ERRONEUSNICKNAME
		client.sendMessage(createReply(Reply::ERR_ERRONEUSNICKNAME,
			client.getNickname(), nick) );
		return ;
	}
	
	client.setNickname(nick);

	if (!client.isRegistered())
		attemptRegistration(client);
	else
		client.sendMessage(":ircserv NICK :" + client.getNickname() + "\r\n");
}

void Server::handleUser(Client& client, const std::string& line)
{
	if (!client.isPassAccepted() )
	{
		client.sendMessage(":ircserv * :Password required before NICK/USER\r\n");
		return ;
	}

	std::vector<std::string> args = getArgsWithColon(line); 

	if (args.size() < 2)
	{ // 461 ERR_NEEDMOREPARAMS
		client.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS,
			"*", "USER") );
		return ;
	}
	
	client.setUsername(args[0]);
	if (args.back()[0] == ':')
		client.setRealname(args.back().substr(1) ); // substr removes ':'
	else
		client.setRealname(args[1]);

	if (!client.isRegistered())
		attemptRegistration(client);
	else // 462 ERR_ALREADYREGISTERED
		client.sendMessage(createReply(Reply::ERR_ALREADYREGISTERED,
			client.getNickname()) );
}