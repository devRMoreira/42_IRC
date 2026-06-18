#include "../inc/irc.hpp"
#include "../inc/constants.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cctype>
#include <sstream>

std::string extractCmd(const std::string& line)
{
	return line.substr(0, line.find(' '));
}

std::string extractArg(const std::string& line)
{
	return line.substr(line.find(' ') + 1);
}

std::vector<std::string> splitString(const std::string& str, char delim)
{
	std::vector<std::string> res;

	size_t i = 0;

	while(i <= str.size())
	{
		size_t pos = str.find(delim, i);

		if (pos == std::string::npos)
		{
			res.push_back(str.substr(i));
			break;
		}

		res.push_back(str.substr(i, pos - i));
		i = pos + 1;
	}

	return res;
}

std::string normalizeString(const std::string& str)
{
	std::string res;
	for(size_t i = 0; i < str.size(); i++)
		res.push_back(std::tolower(str[i]));
	return res;
}

std::vector<std::string> extractMultipleArgs(const std::string& line)
{
	std::vector<std::string> args;

	size_t pos = line.find(' ');

	if(pos == std::string::npos)
		return args;

	pos++;

	while(pos < line.size())
	{
		size_t next = line.find(' ', pos);

		if(next == std::string::npos)
		{
			args.push_back(line.substr(pos));
			break;
		}

		args.push_back(line.substr(pos, next - pos));
		pos = next + 1;
	}

	return args;
}

bool isNumeric(const std::string& str)
{
	for(size_t i = 0; i < str.size(); i++)
	{
		if(!isdigit(str[i]))
			return false;
	}

	return true;
}

std::string intToString(int n)
{
	if (n == 0)
		return "0";

	std::string res;

	while (n > 0)
	{
		res.insert(res.begin(), '0' + (n % 10));
		n /= 10;
	}

	return res;
}

bool areEqualCapitalized(const std::string& str1, const std::string& str2)
{
	if (str1.size() != str2.size())
		return false;
	else
	{
		for (size_t i = 0; i < str1.size(); i++)
		{
			if (toupper(str1[i]) != toupper(str2[i]))
				return(false);
		}
	}
	return true;
}

int initListener(const char* port)
{
	int listenerFd;
	int yes = 1;
	int rv;

	addrinfo hints, *addrinfo, *p;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	rv = getaddrinfo(NULL, port, &hints, &addrinfo);

	if(rv != 0)
		return ServerConstants::ERR_VAL;

	for(p = addrinfo; p != NULL; p = p->ai_next)
	{
		listenerFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(listenerFd < 0)
			continue;

		setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if(bind(listenerFd, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listenerFd);
			continue;
		}

		break;
	}

	if(p == NULL)
		return ServerConstants::ERR_VAL;

	freeaddrinfo(addrinfo);

	if(listen(listenerFd, 10) == ServerConstants::ERR_VAL)
		return ServerConstants::ERR_VAL;

	return listenerFd;
}

std::string convertToAddress(void *address, char *buffer, size_t size)
{
	sockaddr_storage *addressStorage = static_cast<sockaddr_storage*> (address);
	sockaddr_in *v4address;
	sockaddr_in6 *v6address;
	void *src;

	switch(addressStorage->ss_family)
	{
		case AF_INET:
			v4address = static_cast<sockaddr_in*> (address);
			src = &(v4address->sin_addr);
			break;
		case AF_INET6:
			v6address = static_cast<sockaddr_in6*> (address);
			src = &(v6address->sin6_addr);
			break;
		default:
			return std::string("Unknown address type");
	}

	inet_ntop(addressStorage->ss_family, src, buffer, size);
	return std::string(buffer);
}

std::vector<std::string> getArgsWithColon(const std::string& line)
{
	std::vector<std::string> args;
	std::stringstream ss(line);
	std::string arg;

	ss >> arg; // skip command

	while(ss >> arg) // already skips leading whitespace
	{
		if (arg[0] != ':')
			args.push_back(arg);
		else
		{
			if (arg.size() > 1)
			{
				std::string lastArg = arg;
				std::string rest;
				std::getline(ss, rest);
				lastArg += rest;
				args.push_back(lastArg);
			}
			break;
		}	
	}

	return args;
}

std::vector<std::string> getArgs(const std::string& line)
{
	std::vector<std::string> args;
	std::stringstream ss(line);
	std::string arg;

	ss >> arg; // skip command

	while(ss >> arg)
		args.push_back(arg);

	return args;
}