#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

class Client;

//TODO
//Send RPL_TOPIC/NOTOPIC on user joining channel

class Channel
{
	public:
		Channel(const std::string& name);

		void addOperator(Client& client);

		void addClient(Client& client);

		bool isMember(const std::string& nick) const;
		bool isOperator(const std::string& nick) const;

		bool isTopicProtected() const;

		void setTopic(const std::string& str);
		void setKey(const std::string& str);
		void setUserLimit(unsigned int n);
		void setInviteOnly(const bool val);
		void setTopicProtected(const bool val);
		void setOperator(const std::string& nick, bool val);

		std::string getTopic() const;

		std::string getName() const;
		void broadcast(const std::string& msg);
		void broadcast(Client& sender, const std::string& msg);
		void debugChannel() const;

	private:
		struct Member
		{
			Client* client;
			bool isOperator;

			Member(Client *client, bool isOp) : client(client), isOperator(isOp) {}
		};

		std::string _name;
		std::string _topic;
		std::string _key;
		unsigned int _userLimit;
		bool _inviteOnly;
		bool _topicProtected;

		std::vector<Member> _members;

};

#endif