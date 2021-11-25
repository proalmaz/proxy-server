#pragma once

#include "../Proxy/Proxy.hpp"
#define SEND_TO_DB 0
#define SEND_TO_CLIENT 1
#define READ_FROM_CLIENT 2
#define READ_FROM_DB 3
#define CLOSE 4
#define BUFFSIZE 100000

class Client
{
private:
	int 		m_clientFd;
	int 		m_DBFd;
	int 		m_DBport;
	int 		m_opt;
	int 		m_status;
	int 		m_addrlen;
	string		m_DBAddress;
	char 		m_request[BUFFSIZE];
	char 		m_response[BUFFSIZE];
	int 		m_reqLen;
	int 		m_resLen;
	struct sockaddr_in	m_addr;

	Client();

public:
	Client(string &DBAddress, int fd, int DBport);
	~Client();
	Client		&operator=(Client const &copy);

	void 		receiveFromClient();
	void 		receiveFromDB();
	void 		sendToClient();
	void 		sendToDB();

	void 		setStatus(int status);
	int			getStatus();
	int 		getClientFd();
	int 		getDBFd();

};