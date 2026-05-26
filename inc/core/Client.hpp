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

		std::string getUsername() const;

	private:
		int _fd;
		std::string _buffer;
		std::string _nickname;
		std::string _username;
		std::string _realname;
		bool _authenticated;


};


#endif