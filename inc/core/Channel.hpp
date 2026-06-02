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

		void setTopic(const std::string& topic);

		void broadcast(Client& sender, const std::string& msg);
	private:
		struct Member
		{
			Client* client;
			bool isOperator;

			Member(Client *client, bool isOp) : client(client), isOperator(isOp) {}
		};

		std::string _name;
		std::string _topic;

		std::vector<Member> _clients;
};

#endif