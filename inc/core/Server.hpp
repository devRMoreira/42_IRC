#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <map>
#include <string>
#include <vector>

class Channel;
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

		std::map<std::string, Channel*> _channels;

		int _listenerFd;

		void connectClient(int fd);
		void disconnectClient(int index);
		void handleNewConnection();
		void handleClientData(int& i);
		void removePfd(int fd);

};


#endif