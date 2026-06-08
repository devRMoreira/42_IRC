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
		ERR_USERNOTINCHANNEL = 441,
		ERR_NEEDMOREPARAMS = 461,
		ERR_CHANOPRIVSNEEDED = 482,
		ERR_UMODEUNKNOWNFLAG = 501
	};

};

#endif