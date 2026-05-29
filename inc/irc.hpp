#ifndef IRC_HPP
#define IRC_HPP

#include <string>

int initListener(const char* port);

std::string convertToAddress(void *address, char *buffer, size_t size);
std::string extractCmd(const std::string& line);
std::string extractArg(const std::string& line);

#endif