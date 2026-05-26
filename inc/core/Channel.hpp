#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

class Client;

class Channel
{
	public:

	private:
		std::string _name;

		std::vector<Client *> _clients;
		std::vector<Client *> _operators;

};

#endif