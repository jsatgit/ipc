#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>

#define UNIX_PATH_MAX 104
#define BUF_MAX 512 

/* socket */
int sockfd;

void fd_destroy(int sockfd) {
	int res;
	if ((res = close(sockfd)) == -1) {
		perror("Unable to close socket.\n");
		exit(EXIT_FAILURE);
	}
}

void sighandler(int signum) {
	if (signum == SIGTSTP) {
		fd_destroy(sockfd);
		printf("Sucessfully quit application\n");
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char** argv) {
	char* unix_path;
	int res;
	struct sockaddr_un addr_un;
	struct sockaddr_in addr_in;
	struct sockaddr from;
	struct sockaddr* addr;
	socklen_t fromlen;
	size_t len;
	char buf[BUF_MAX];
	ssize_t recv_res;
	struct sigaction act;
	int is_un, is_in;
	int domain;
	in_port_t portno;
	char* ipAddr;

	/* set up signal handler */
	memset(&act, 0, sizeof(act));
	act.sa_handler = sighandler;

	if ((res = sigaction(SIGTSTP, &act, NULL)) == -1) {
		perror("Cannot register signal handler.\n");
		exit(EXIT_FAILURE);
	}

	/* options count */
	if (argc < 2) {
		perror("Invalid number of arguments.\n");
		exit(EXIT_FAILURE);
	}

	/* get domain */
	is_un = is_in = 0;
	if ((res = strcmp(argv[1], "-u")) == 0) {
		is_un = 1;
		domain = AF_UNIX;
	}
	else if ((res = strcmp(argv[1],"-i")) == 0) {
		is_in = 1;
		domain = AF_INET;
	}
	else {
		perror("Invalid option.\n");
		exit(EXIT_FAILURE);
	}

	/* Check addr arguments */
	if (is_in) {
		if (argc != 4) {
			perror("Invalid number of arguments.\n");
			exit(EXIT_FAILURE);
		}	
	}
	else if (is_un) {
		if (argc != 3) {
			perror("Invalid number of arguments.\n");
			exit(EXIT_FAILURE);
		}	
	}

	/* get addr arguments */
	if (is_in) {
		ipAddr = argv[2];	

		// check for this ?
		portno = atoi(argv[3]); 
	}
	else if (is_un) {
		/* get path */
		unix_path = argv[2];
		if ((len = strlen(unix_path)) > UNIX_PATH_MAX) {
			perror("Invalid unix path length.\n");
			exit(EXIT_FAILURE);
		} 
	}

	/* create socket */
	if ((sockfd = socket(domain, SOCK_DGRAM, 0)) == -1) {
		perror("Unable to create a socket.\n");
		exit(EXIT_FAILURE);
	}

	/* fill addr structs */
	addr = NULL;
	if (is_in) {
		memset(&addr_in, 0, sizeof(addr_in));
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons(portno); 
		if ((res = inet_pton(AF_INET, ipAddr, &addr_in.sin_addr)) != 1) {
			perror("Unable to translate ip address\n");
			exit(EXIT_FAILURE);
		}
		addr = (struct sockaddr *)&addr_in;
	}
	else if (is_un) {
		memset(&addr_un, 0, sizeof(addr_un));
		addr_un.sun_family = AF_UNIX;
		strcpy(addr_un.sun_path, unix_path);
		addr = (struct sockaddr *)&addr_un;
	}

	/* assign name to socket */
	if ((res = bind(sockfd, addr, sizeof(*addr))) == -1) {
		perror("Unable to assign a name to a socket.\n");
		fd_destroy(sockfd);
		exit(EXIT_FAILURE);
	}

	/* main loop */
	while (1) {
		if ((recv_res = recvfrom(sockfd, buf, sizeof(buf), 0, 
								 &from, &fromlen)) == -1) {
			perror("Unable to receive message.\n");
			fd_destroy(sockfd);
			exit(EXIT_FAILURE);
		}

		printf("Sender: %s\n", buf);

		/* clear buffer */
		memset(buf, 0, sizeof(buf));
	}
}
