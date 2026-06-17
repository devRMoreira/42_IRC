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
	{ // 462 ERR_ALREADYREGISTERED
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
	arg = getArgs(arg)[0];

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
		attemptRegistration(client);
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

	std::string argStr = extractArg(line);
	std::vector<std::string> args = getArgsWithColon(argStr);

	if (args.size() < 2)
	{// 461 ERR_NEEDMOREPARAMS
		client.sendMessage(createReply(Reply::ERR_NEEDMOREPARAMS, client.getNickname(), "USER") );
		return ;
	}
	
	client.setUsername(args[0]);
	client.setRealname(args[1]); // could be args.back() if it detects ':' to better mimic IRC

	if (!client.isRegistered())
		attemptRegistration(client);
	else // 462 ERR_ALREADYREGISTERED
		client.sendMessage(createReply(Reply::ERR_ALREADYREGISTERED, client.getNickname()) );
}