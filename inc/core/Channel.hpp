#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

class Client;

class Channel
{
	public:
		Channel(const std::string& name);

		void addOperator(Client& client);
		void addClient(Client& client);

	private:
		std::string _name;

		std::vector<Client *> _clients;
		std::vector<Client *> _operators;

};

#endif