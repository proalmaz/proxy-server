#include "Proxy.hpp"

Proxy::Proxy() {}

Proxy::Proxy(string &ipAddress, int port, int portBD) : m_ipAddress
(ipAddress), m_port(port), m_BDport(portBD) {}

Proxy::~Proxy()
{
	close(m_fd);
	for (int i = 0; i < m_clients.size(); ++i)
	{
		close(m_clients[i]->getClientFd());
		close(m_clients[i]->getDBFd());
	}
	cout << "All connections are closed" << endl;
}

void Proxy::socketPreparation()
{
	// создание фд сокета
	m_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_fd == -1)
		throw string("Error: socket");
	m_opt = 1;
	if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &m_opt, (socklen_t)(sizeof
	(m_opt)) == -1))
		throw string("Error: setsockopt");
	// мы говорим, что будем использовать ТСP протокол
	m_addr.sin_family = AF_INET;
	// порт любой, кроме (< 1024 и частных > 49151)
	m_addr.sin_port = htons(m_port);
	// 127.0.0.1 = INADDR_ANY
	m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// связываем фд сокета со структурой адреса
	if (bind(m_fd, (struct sockaddr *)&m_addr, sizeof(m_addr)) == -1)
	{
		clock(m_fd);
		throw string("Error: bind\n");
	}
	int flag = fcntl(m_fd, F_GETFL);
	fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
	cout << "Proxy-server created with port " << m_port << endl;
}

void Proxy::listenSock(int backlog)
{
	// переводим сокет в режим прослушки, backlog - кол-во сокетов для
	// одновременного прослушивания
	if (listen(m_fd, backlog) != 0)
		throw "Listen socket failed";
	cout << "Server is listening port " << m_port << endl;
}

int Proxy::getAddressLen() {return m_addrlen;}

int Proxy::getFd() {return m_fd;}

int Proxy::getPort() {return m_port;}

void Proxy::putFdSpace()
{
	// очищаем фдшники для чтения и записи
	FD_ZERO(&m_readFds);
	FD_ZERO(&m_writeFds);
	FD_SET(m_fd, &m_readFds);
}

void Proxy::run()
{
	int maxFd = m_fd;
	int clientFd, DBFd, res;
	while (1)
	{
		putFdSpace();
	}
}