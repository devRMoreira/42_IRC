#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

void Server::handleNick(Client& client, const std::string& line)
{
	if (!client.isPassAccepted() )
	{
		client.sendMessage(client.getPrefix() + "* :Password required before NICK/USER\r\n");
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