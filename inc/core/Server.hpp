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

		//TODO change client to non pointer, it doesnt make sense
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

		Client* nickExists(const std::string& nick);
		// Channel* channelExists(const std::string& channel);

		void attemptRegistration(Client& client);
		void handleJoin(Client& client, const std::string& line);
		void parseMode(Client& client, const std::string& line);

		void createNewChannel(Client&  client, const std::string& arg);
		void clientJoinChannel(Client&  client, const std::string& arg);

		bool channelExists(const std::string& str);
;


};


#endif