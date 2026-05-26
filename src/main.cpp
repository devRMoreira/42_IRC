#include "../inc/core/Server.hpp"
#include "../inc/constants.hpp"
#include <string>
#include <iostream>

int main(int argc, char**argv)
{
	if(argc != ServerConstants::TOTAL_ARGS)
	{
		std::cerr << "Usage: ./server <port> <password>\n";
		return 1;
	}

	Server server(argv[1], argv[2]);
	std::cout << "Server ready\n";

	bool exit = false;

	while(!exit)
	{
		int res = poll(server.getListener(), server.getFdCount(), -1);

		if(res == ServerConstants::ERR_VAL)
		{
			std::cerr << "poll() error\n";
			exit = true;
		}
		else
			server.run();
	}

	return exit;
}