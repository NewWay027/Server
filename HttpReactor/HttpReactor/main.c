#include <stdio.h>
#include <unistd.h>
#include "TcpServer.h"

int main(int argc, char * argv[])
{
	unsigned int port = 10000;
	chdir("/home/newway027/luffy");

	struct TcpServer * server =  TcpServerInit(port, 0);
	TcpServerRun(server);
}