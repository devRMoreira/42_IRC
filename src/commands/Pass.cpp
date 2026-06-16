#include "../../inc/core/Server.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"
#include "../../inc/constants.hpp"
#include "../../inc/message.hpp"

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
			// client.sendMessage(":ircserv 464 " + client.getNickname() + " :Password incorrect\r\n");
		}
	}
	else
	{
	//	462 ERR_ALREADYREGISTERED
		client.sendMessage(createReply(Reply::ERR_ALREADYREGISTERED, client.getNickname()));
	}
}