#include "../inc/constants.hpp"

#include <string>
#include <sstream>

static std::string buildReply(Reply::Code code, const std::string& client, const std::string& p1, const std::string& p2)
{
	std::string msg = ":ircserv ";

	std::ostringstream oss;
	oss << code;
	msg += oss.str() + " ";

	switch (code)
	{
		case(Reply::RPL_NOTOPIC):
			msg += client + " " + p1 + " :No topic is set";
			break;

		case(Reply::RPL_TOPIC):
			msg += client + " " + p1 + " :" + p2;
			break;

		case(Reply::RPL_NAMREPLY):
			msg += client + " " + p1;
			break;

		case(Reply::RPL_ENDOFNAMES):
			msg += client + " " + p1 + " :End of /NAMES list";
			break;

		case(Reply::ERR_NOSUCHCHANNEL):
			msg += client + " " + p1 + " :No such channel";
			break;

		case(Reply::ERR_USERNOTINCHANNEL):
			msg += client + " " + p1 + " " + p2 + " :They aren't on that channel";
			break;

		case(Reply::ERR_NOTREGISTERED):
			msg += client + ":You have not registered";
			break;

		case(Reply::ERR_NOTONCHANNEL):
			msg += client + " " + p1 + " :You're not on that channel";
			break;

		case(Reply::ERR_NEEDMOREPARAMS):
			msg += client + " " + p1 + " :Not enough parameters";
			break;

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


