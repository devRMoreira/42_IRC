#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

void Server::handleUser(Client& client, const std::string& line) // needs more checks
{
	if (!client.isPassAccepted() )
	{
		client.sendMessage(client.getPrefix() + "* :Password required before NICK/USER\r\n");
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