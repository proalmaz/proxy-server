#include "Proxy.hpp"

char	*countTime()
{
	time_t cur;
	cur = time(NULL);
	return ctime(&cur);
}

void	putToLogFile(char *packet)
{
	char	*date = countTime();
	int 	fd = open("proxy.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd < 0)
	{
		string error = "Error: can't open or create log file: " +
				string(strerror(errno));
		throw error;
	}
    int i = 0;
	string output = "\n" + string(date) + "\n" + &packet[5] +
			"\n------------";
	write(fd, output.c_str(), output.length());
	close(fd);
}
