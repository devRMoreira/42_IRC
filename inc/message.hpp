#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "./constants.hpp"
#include <string>

std::string createReply(Reply::Code code, const std::string& client);
std::string createReply(Reply::Code code, const std::string& client, const std::string& p1);
std::string createReply(Reply::Code code, const std::string& client , const std::string& p1, const std::string& p2);

#endif