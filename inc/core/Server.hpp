#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"

#include <poll.h>
#include <map>
#include <string>
#include <vector>


class Client;

class Server
{
	public:
		Server(std::string port, std::string pw);
		~Server();

		void run();

		pollfd* getListener();
		int getFdCount() const;

	private:
		std::vector<pollfd> _pfds;
		std::string _password;
		std::string _port;

		std::map<int, Client*> _clients;

		std::map<std::string, Channel> _channels;

		int _listenerFd;

		void connectClient(int fd);
		void disconnectClient(int index);
		void handleNewConnection();
		void handleClientData(int& i);
		void removePfd(int fd);

		void handleLine(Client& client, const std::string& line);

		void handleCap(Client& client, const std::string& line);
		void handlePass(Client& client, const std::string& line);
		void handleNick(Client& client, const std::string& line);
		void handleUser(Client& client, const std::string& line);
		void handlePrivMsg(Client& client, const std::string& line);
		void handleInvite(Client& client, const std::string& line);
		void handleKick(Client& client, const std::string& line);



		void attemptRegistration(Client& client);
		void handleJoin(Client& client, const std::string& line);
		void parseMode(Client& client, const std::string& line);
		void handleMode(Client& client, Channel& channel, std::string modeString, std::vector<std::string> params);

		void handleTopic(Client& client, const std::string& line);

		void createNewChannel(Client&  client, const std::string& arg);
		void clientJoinChannel(Client&  client, const std::string& arg);

		Channel* getChannel(const std::string& name);
		Client* getClient(const std::string& nick);
};


#endif