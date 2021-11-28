#pragma once

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <ctime>
#include <algorithm>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>

using std::cin;
using std::string;
using std::cout;
using std::endl;
using std::cerr;

class Proxy
{
private:
	int 					m_port;
	int 					m_BDport;
	int 					m_opt;
	string					m_ipAddress;
	struct sockaddr_in		m_addr;
	int 					m_addrlen;
	int 					m_fd;
	fd_set					m_writeFds;
	fd_set					m_readFds;
	std::vector<Client>		m_clients;
	Proxy();

public:
	Proxy(string &ipAddress, int port, int portBD);
	~Proxy();

	void 					listenSock(int backlog);
	void 					socketPreparation();
	void 					run();
	void 					putFdSpace();

	int						getPort();
	int 					getAddressLen();
	int 					getFd();
};

void						putToLogFile(char *packet);