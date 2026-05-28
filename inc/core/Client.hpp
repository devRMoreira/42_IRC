#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client
{
	public:
		Client(int fd);

		void addToBuffer(std::string data);
		std::vector<std::string> getLines();

		void setNickname(const std::string& nickname);
		std::string getNickname() const;

		void setUsername(const std::string& username);
		std::string getUsername() const;

		bool getCapEnd() const;
		void setCapEnd();

		void setPassAccepted();
		int sendMessageToClient(const std::string& msg);

		bool isRegistered() const;

	private:
		int _fd;
		std::string _buffer;
		std::string _nickname;
		std::string _username;
		std::string _realname;

		bool _passAccepted;
		bool _capEnd;
		bool _registered;


};


#endif