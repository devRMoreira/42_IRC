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

#endif