#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace ServerConstants
{
    enum
    {
        TOTAL_ARGS = 3,
        BUFFER_SIZE = 512,
        ERR_VAL = -1
    };
}

namespace ChannelConstants
{
    enum
    {
        MAX_NAME_SIZE = 200,
        MAX_CLIENTS = 32767
    };
}

namespace ModeFlag
{
    enum
    {
        INVALID = 0,
        INVITE = 'i',
        TOPIC = 't',
        KEY = 'k',
        OPERATOR = 'o',
        USER_LIMIT = 'l',
        ADD = '+',
        REMOVE = '-'
    };

}

namespace Reply
{
	enum Code
	{
		RPL_WELCOME = 1,
		RPL_CHANNELMODEIS = 324,
		RPL_NOTOPIC = 331,
		RPL_TOPIC = 332,
        RPL_INVITING = 341,
		RPL_NAMREPLY = 353,
		RPL_ENDOFNAMES = 366,
        ERR_NOSUCHNICK = 401,
		ERR_NOSUCHCHANNEL = 403,
        ERR_NOTEXTTOSEND = 412,
        ERR_UNKNOWNCOMMAND = 421,
        ERR_NONICKNAMEGIVEN = 431,
        ERR_ERRONEUSNICKNAME = 432,
        ERR_NICKNAMEINUSE = 433,
		ERR_USERNOTINCHANNEL = 441,
        ERR_NOTONCHANNEL = 442,
        ERR_USERONCHANNEL = 443,
		ERR_NOTREGISTERED = 451,
		ERR_NEEDMOREPARAMS = 461,
        ERR_ALREADYREGISTERED = 462,
        ERR_PASSWDMISMATCH = 464,
        ERR_INVITEONLYCHAN = 473,
		ERR_BADCHANNELKEY = 475,
		ERR_CHANOPRIVSNEEDED = 482,
		ERR_UMODEUNKNOWNFLAG = 501,

	};

};

#endif