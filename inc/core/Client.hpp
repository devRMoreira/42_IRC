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

		bool hasNick() const;
		void setNickname(const std::string& nickname);
		std::string getNickname() const;

		bool hasUsername() const;
		void setUsername(const std::string& username);
		std::string getUsername() const;

		void setRealname(const std::string& realname);
		std::string getRealname() const;

		std::string getPrefix() const;

		void setCapEnd();
		bool isCapEnd() const;

		void setPassAccepted();
		bool isPassAccepted() const;

		int sendMessage(const std::string& msg) const;

		bool canRegister() const;
		bool isRegistered() const;
		void setRegistered(bool val);



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