#include "Proxy/Proxy.hpp"

void checkArgs(int argc, char **argv, int port)
{
	if (argc != 4)
	{
		cerr << "Error: wrong number of arguments.\nEnter ip address of "
				"BD-server, port of BD-server and proxy-server port:\n"
				"For example: ./proxy 127.0.0.1 3306 8080\n";
		exit(EXIT_FAILURE);
	}
	if (inet_addr(argv[1]) == INADDR_NONE)
	{
		cerr << "Error: wrong ip address of BD-server\n";
		exit(EXIT_FAILURE);
	}
	if (port < 1024 || port > 49151)
	{
		std::cout << "Wrong port!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	int proxyPort = atoi(argv[3]);
	checkArgs(argc, argv, proxyPort);
	try
	{
		string ip = argv[1];
		int portBD = atoi(argv[2]);
		if (proxyPort <= 0 || portBD <= 0)
			throw string("Wrong ip");
		Proxy serv(ip, proxyPort, portBD);
		serv.socketPreparation();
		serv.listenSock(128);
		serv.run();
	}
	catch (string &error)
	{
		cerr << error << endl;
	}
	return 0;
}
