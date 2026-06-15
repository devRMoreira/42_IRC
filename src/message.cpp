#include "../inc/constants.hpp"

#include <string>
#include <sstream>
#include <iomanip>


#include <iostream>

static std::string buildReply(Reply::Code code, const std::string& client, const std::string& p1, const std::string& p2)
{
	std::string msg = ":ircserv ";

	std::ostringstream oss;
	oss << std::setw(3) << std::setfill('0') << code;
	msg += oss.str() + " ";

	switch (code)
	{
		case(Reply::RPL_WELCOME):
			msg += client + " :Welcome to the server" + p1;
			break;

		case(Reply::RPL_CHANNELMODEIS):
			msg += client + " " + p1 + " " + p2;
			break;

		case(Reply::RPL_NOTOPIC):
			msg += client + " " + p1 + " :No topic is set";
			break;

		case(Reply::RPL_TOPIC):
			msg += client + " " + p1 + " :" + p2;
			break;

		case(Reply::RPL_INVITING):
			msg += client + " " + p1 + " " + p2;
			break;

		case(Reply::RPL_NAMREPLY):
			msg += client + " = " + p1 + " :" + p2;
			break;

		case(Reply::RPL_ENDOFNAMES):
			msg += client + " " + p1 + " :End of /NAMES list";
			break;

		case(Reply::ERR_NOSUCHNICK):
			msg += client + " " + p1 + " :No such nick/channel";
			break;

		case(Reply::ERR_NOSUCHCHANNEL):
			msg += client + " " + p1 + " :No such channel";
			break;

		case(Reply::ERR_NOTEXTTOSEND):
			msg += client + " " + " :No text to send";
			break;

		case(Reply::ERR_UNKNOWNCOMMAND):
			msg += client + " " + p1 + " :Unknown command";
			break;

		case(Reply::ERR_NONICKNAMEGIVEN):
			msg += client + " :No nickname given";
			break;

		case(Reply::ERR_ERRONEUSNICKNAME):
			msg += client + " " + p1 + " :Erroneous nickname";
			break;

		case(Reply::ERR_NICKNAMEINUSE):
			msg += client + " " + p1 + " :Nickname is already in use";
			break;

		case(Reply::ERR_USERNOTINCHANNEL):
			msg += client + " " + p1 + " " + p2 + " :They aren't on that channel";
			break;

		case(Reply::ERR_NOTONCHANNEL):
			msg += client + " " + p1 + " :You're not on that channel";
			break;

		case(Reply::ERR_USERONCHANNEL):
			msg += client + " " + p1 + " " + p2 + " :is already on channel";

		case(Reply::ERR_NOTREGISTERED):
			msg += client + ":You have not registered";
			break;

		case(Reply::ERR_NEEDMOREPARAMS):
			msg += client + " " + p1 + " :Not enough parameters";
			break;

		case(Reply::ERR_ALREADYREGISTERED): 
			msg += client + " :You may not reregister";
			break;

		case(Reply::ERR_PASSWDMISMATCH):
			msg += "* :Password incorrect";
			break;

		case(Reply::ERR_INVITEONLYCHAN):
			msg += client + " " + p1 + " :Cannot join channel (+i)";

		case(Reply::ERR_BADCHANNELKEY):
			msg += client + " " + p1 + " :Cannot join channel (+k)";
			break;

		case(Reply::ERR_CHANOPRIVSNEEDED):
			msg += client + " " + p1 + " :You're not channel operator";
			break;

		case(Reply::ERR_UMODEUNKNOWNFLAG):
			msg += client + " :Unknown MODE flag";
			break;

		default:
			msg += "";
	}

	msg += "\r\n";

	std::cout << "\nNUM REPLY SENT\n" << msg << std::endl;

	return msg;
}

std::string createReply(Reply::Code code, const std::string& client)
{
	return buildReply(code, client, "", "");
}
std::string createReply(Reply::Code code, const std::string& client, const std::string& p1)
{
	return buildReply(code, client, p1, "");
}

std::string createReply(Reply::Code code, const std::string& client , const std::string& p1, const std::string& p2)
{
	return buildReply(code, client, p1, p2);
}


