#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(string &DBAddress, int fd, int DBport) : m_DBAddress
(DBAddress), m_clientFd(fd), m_DBport(DBport)
{
	m_DBFd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_DBFd < 0)
		throw string("Error: socket of client failed");
	m_opt = 1;
	if (setsockopt(m_DBFd, SOL_SOCKET, &m_opt,
				   (socklen_t)(sizeof (m_opt))) == -1)
		throw string("Error: setsockopt of client failed");
	m_addrlen = sizeof(m_addr);
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(DBport);
	// inet_aton() преобразовывает обычный вид IP-адреса m_DBAddress.c_str()
	// (из номеров и точек) в двоичный код и сохраняет его в структуре, на
	// которую указывает m_addr.sin_addr.
	if (!inet_aton(m_DBAddress.c_str(), &(m_addr.sin_addr)))
		throw string("Error: wrong ip address");
	// connect - инициирует соединение на сокете
	if (connect(m_DBFd, (struct sockaddr *)&m_addr, (socklen_t)m_addrlen))
		throw string("Error: connect to DB failed");
	int flag = fcntl(m_DBFd, F_SETFL, flag | O_NONBLOCK);
	m_status = READ_FROM_DB;
}

Client &Client::operator=(const Client &copy)
{
	m_clientFd = copy.m_clientFd;
	m_DBFd = copy.m_DBFd;
	m_DBport = copy.m_DBport;
	m_opt = copy.m_DBport;
	m_status = copy.m_status;
	m_addrlen = copy.m_addrlen;
	m_addr = copy.m_addr;
	m_DBAddress = copy.m_DBAddress;
	memcpy(m_request, copy.m_request, sizeof(m_request));
	memcpy(m_response, copy.m_response, sizeof(m_response));
	m_reqLen = copy.m_reqLen;
	m_resLen = copy.m_resLen;
	return (*this);
}

void Client::setStatus(int status) { m_status = status; }

int Client::getDBFd() { return m_DBFd; }

int Client::getClientFd() { return m_clientFd; }

int Client::getStatus() { return m_status; }

void Client::receiveFromClient()
{
	bzero(m_request, BUFFSIZE);
	int len = recv(m_clientFd, m_request, sizeof(m_request), 0);
	if (len < 0)
	{
		m_status = CLOSE;
		string error = "Error: receive from client failed: " +
				strerror(string(errno));
		throw error;
	}
	else if (!len)
	{
		m_status = CLOSE;
		cout << "Connection with socket was closed" << endl;
		return;
	}
	m_reqLen = len;
	if (int(m_request[4]) == COM_STM_PREPARE || int(m_request[4]) == COM_QUERY)
	{
		try
		{
			putToLogFile(m_request);
		}
		catch (string &error)
		{
			cerr << error << endl;
		}
	}
	m_status = SEND_TO_DB;
}

void Client::receiveFromDB()
{
	bzero(m_response, BUFFSIZE);
	int len = recv(m_DBFd, m_response, sizeof(m_response), 0);
	if (len < 0)
	{
		string error = "Error: receive from DB failed: " +
				string(strerror(errno));
		m_status = CLOSE;
		throw error;
	}
	else if (len == 0)
	{
		cout << "Connection was closed" << endl;
		m_status = CLOSE;
		return;
	}
	m_resLen = len;
	m_status = SEND_TO_CLIENT;
}

void Client::sendToDB()
{
	if (send(m_DBFd, m_request, m_reqLen, 0) < 0)
	{
		string error = "Error: send to DB failed: " +
				string(strerror(errno));
		m_status = CLOSE;
		throw error;
	}
	m_status = READ_FROM_DB;
}

void Client::sendToClient()
{
	if (send(m_clientFd, m_response, m_resLen, 0) < 0)
	{
		string error = "Error: send to client failed: " +
				string(strerror(errno));
		m_status = CLOSE;
		throw error;
	}
	m_status = READ_FROM_CLIENT;
}