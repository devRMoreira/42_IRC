#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

class Client;

class Channel
{
	public:
		Channel(const std::string& name);

		void addClient(Client& client);
		void removeClient(Client& client);

		void addClientInvite(const Client* client);
		void removeClientInvite(const Client* client);
		bool isClientInvited(const Client* client);

		bool isOperator(const Client* client) const;
		void addOperator(Client& client);
		void setOperator(const Client* client, bool val);

		bool isMember(const Client* client) const;

		std::string getModeString() const;

		bool isTopicProtected() const;
		void setTopicProtected(const bool val);

		std::string getTopic() const;
		void setTopic(const std::string& str);

		bool isKeyProtected() const;

		bool isInviteOnly() const;
		void setInviteOnly(const bool val);

		bool hasUserLimit() const;
		unsigned int getUserLimit() const;
		void setUserLimit(unsigned int n);

		std::string getKey() const;
		void setKey(const std::string& str);

		std::string getName() const;

		std::string getMemberNames() const;

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
		std::vector<const Client *> _invitedClients;

};

#endif