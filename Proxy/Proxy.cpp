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
		free(m_clients[i]);
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
	(m_opt))) == -1)
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
		close(m_fd);
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

void Proxy::deleteClient(Client *client)
{
	for (int i = 0; i < m_clients.size(); ++i)
	{
		if (m_clients[i] == client)
		{
			m_clients.erase(m_clients.begin() + i);
			return;
		}
	}
}

void Proxy::acceptNewClient()
{
	bzero(&m_addr, sizeof(m_addr));
	int newClientFd = accept(m_fd, (struct sockaddr *)&m_addr, (socklen_t *)
			&m_addrlen);
	if (newClientFd < 0)
	{
		string error =  "Error: accept new client failed " + string(strerror(errno));
		throw error;
	}
	setsockopt(newClientFd, SOL_SOCKET, SO_NOSIGPIPE, &m_opt, (socklen_t)
	sizeof(m_opt));
	setsockopt(newClientFd, SOL_SOCKET, SO_REUSEADDR, &m_opt, (socklen_t)
	sizeof(m_opt));
	int flag = fcntl(newClientFd, F_GETFL);
	fcntl(newClientFd, F_SETFL, flag | O_NONBLOCK);
	m_clients.push_back(new Client(m_ipAddress, newClientFd, m_BDport));
	cout << "New connection with client was create " <<
	inet_ntoa(m_addr.sin_addr) << ": " << htons(m_addr.sin_port) << endl;
}

void Proxy::checkClientsFd()
{
	int clientFd, DBFd;
	for (auto client : m_clients)
	{
		clientFd = client->getClientFd();
		DBFd = client->getDBFd();
		if (FD_ISSET(clientFd, &m_readFds))
			client->receiveFromClient();
		if (FD_ISSET(DBFd, &m_readFds))
			client->receiveFromDB();
		if (FD_ISSET(clientFd, &m_writeFds))
			client->sendToClient();
		if (FD_ISSET(DBFd, &m_writeFds))
			client->sendToDB();
	}
}

void Proxy::putFdSpace()
{
	// очищаем фдшники для чтения и записи
	FD_ZERO(&m_readFds);
	FD_ZERO(&m_writeFds);
	FD_SET(m_fd, &m_readFds);
	m_max_fd = m_fd;
	// заполняем множество
	for (auto client : m_clients)
	{
		int status = client->getStatus();
		switch (status)
		{
			case READ_FROM_CLIENT:
				FD_SET(client->getClientFd(), &m_readFds); break;
			case READ_FROM_DB:
				FD_SET(client->getDBFd(), &m_readFds); break;
			case SEND_TO_CLIENT:
				FD_SET(client->getClientFd(), &m_writeFds); break;
			case SEND_TO_DB:
				FD_SET(client->getDBFd(), &m_readFds); break;
			default:
				close(client->getClientFd());
				close(client->getDBFd());
				deleteClient(client);
		}
		m_max_fd = std::max(m_max_fd, std::max(client->getClientFd(), client->getDBFd()));
	}
}

void Proxy::run()
{
	while (1)
	{
		try
		{
			putFdSpace();
			if (select(m_max_fd + 1, &m_readFds, &m_writeFds, 0, 0) < 0)
				throw string("Error: select failed " + string(strerror(errno)));
			if (FD_ISSET(m_fd, &m_readFds))
				acceptNewClient();
			else
				checkClientsFd();
		}
		catch (string &error)
		{
			cerr << error << endl;
		}
	}
}