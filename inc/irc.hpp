#ifndef IRC_HPP
#define IRC_HPP

#include <string>
#include <vector>

int initListener(const char* port);

std::string convertToAddress(void *address, char *buffer, size_t size);
std::string extractCmd(const std::string& line);
std::string extractArg(const std::string& line);
std::vector<std::string> extractMultipleArgs(const std::string& line);
std::vector<std::string> splitString(const std::string& str, char delim);
std::string normalizeString(const std::string& str);
std::vector<std::string> getArgsWithColon(const std::string& str);
std::vector<std::string> getArgs(const std::string& str);

bool areEqualCapitalized(const std::string& str1, const std::string& str2);

bool isNumeric(const std::string& str);
std::string intToString(int n);

#endif