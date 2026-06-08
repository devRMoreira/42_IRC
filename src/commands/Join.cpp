#include "../../inc/core/Server.hpp"
#include "../../inc/core/Channel.hpp"
#include "../../inc/core/Client.hpp"
#include "../../inc/irc.hpp"

#include <string>

//* Is the client registered? If not, reject it
//* Does the channel exist?
//* No → create it, the client becomes its first member and automatically an operator
//* Yes → proceed
//* Does the channel have any restrictions?
//* Invite only → is this client on the invite list?
//* Has a password → did they send the right one?
//* Has a user limit → is the channel full?
//* If all checks pass, add the client to the channel's member list
//* Tell everyone in the channel that this client joined — including the client themselves
//* Send the new client the channel topic if there is one
//* Send the new client the list of who is currently in the channel

#include <iostream>
#include <map>
#include <iterator>

static void printChannels(std::map<std::string, Channel> channels)
{
	std::cout << "Channel list\n" << std::endl;
	int counter = 1;

	for(std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		std::cout << counter++ <<" - "<< it->first << std::endl;
	}
}

void Server::clientJoinChannel(Client&  client, const std::string& arg)
{
	_channels.at(arg).addClient(client);
}

void Server::createNewChannel(Client&  client, const std::string& arg)
{
	_channels.insert(std::make_pair(arg, Channel(arg)));
	_channels.at(arg).addOperator(client);
}

void Server::handleJoin(Client& client, const std::string& line)
{

	std::vector<std::string> args = extractMultipleArgs(line);

	for(size_t i = 0; i < args.size(); i++)
	{
		if(_channels.find(args[i]) == _channels.end())
			createNewChannel(client, args[i]);
		else
			clientJoinChannel(client, args[i]);

		printChannels(_channels);

		client.sendMessageToClient(":ircserv " + client.getUsername() + " :joined " + args[i] + "\r\n");
	}
}
