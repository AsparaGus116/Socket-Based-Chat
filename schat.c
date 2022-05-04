#include <stdio.h>  
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <unistd.h>
#include <stdlib.h> //atoi 
#include <string.h>
#include <poll.h>

void handleChat(int myfd)
{
	char* buf = calloc(4096, sizeof(char));
	struct pollfd fds[2];
	int timeout_msecs = 60000;
	int ret;
	int i;
	/* Open STREAMS device. */
	fds[0].fd = myfd;
	fds[1].fd = 0;
	fds[0].events = POLLIN;
	fds[1].events = POLLIN;
	for(;;)
	{
		ret = poll(fds, 2, timeout_msecs);
		if(fds[0].revents & POLLIN) // socket
		{
			int size = read(myfd, buf, 4096);
			write(0, buf, size);
		}
		if(fds[1].revents & POLLIN)
		{
			int size = read(0, buf, 4096);
			write(myfd, buf, size);
		}
		if(ret <= 0)
		{
			return;
		}
		memset(buf, 0, 4096);
	}
	 
}

int main(int argc , char *argv[]){
	char* ip = "127.0.0.1";
	int port = 0;
    	int server = 1;
	if(argc == 3)
	{
		server = 0;
		ip = argv[1];
		port = atoi(argv[2]);
	}
	else
	{
		srandom(getpid()); // random seed based on this process's OS-assigned ID
    		port = 0xc000 | (random()&0x3fff); // random element of 49152–65535
	}
	
	struct sockaddr_in ipOfServer;
    memset(&ipOfServer, 0, sizeof(struct sockaddr_in));
    ipOfServer.sin_family = AF_INET;
    ipOfServer.sin_addr.s_addr = htonl(INADDR_ANY);
    ipOfServer.sin_port = htons(port);
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	switch(server)
	{
	    if(sock == -1)
	    {
		printf("ERR: SOCKET INITIALIZATION FAILED");
		return -1;
	    }
	case 0:
	   	if(connect(sock, (const struct sockaddr*)&ipOfServer, sizeof(ipOfServer)) ==  -1)
		{
			printf("ERR: FAILED TO CONNECT");
			return -1;
		}
		handleChat(sock);
		
	break;
	case 1:
		// and we need to tell the OS that this socket will use the address created for it
    		bind(sock, (struct sockaddr*)&ipOfServer , sizeof(ipOfServer));

    		// wait for connections; if too many at once, suggest the OS queue up 20
    		listen(sock , 20);

    		printf("The server is now listening on port %d\n", port); // and listening port

    		int connection = accept(sock, 0,0);
			handleChat(connection);
		close(sock);
	break;
	}
}
