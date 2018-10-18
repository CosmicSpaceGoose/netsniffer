#include "netsniffer_d.h"

static void	err_msg(int fd, const char *msg) {
	dprintf(fd, msg, strerror(errno));
	exit(1);
}

void	signal_handler(int signo)
{
	if (signo == SIGINT)
		;
}

static void	setsignals(void)
{
	sigset_t sigset;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGQUIT);
	sigaddset(&sigset, SIGINT);
	sigaddset(&sigset, SIGTERM);
	sigaddset(&sigset, SIGCHLD);
	sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);
}

static void	starter(int fd)
{
	switch (fork())
	{
		case 0:
			break ;
		case -1:
			// syslog(LOG_ERR, "Failed to fork process:  %s", strerror(errno));
			err_msg(fd, "Failed to fork process: %s");
		default:
			exit(0);
	}
	if (setsid() < 0)
	{
		// syslog(LOG_ERR, "Failed to setsid():  %s", strerror(errno));
		err_msg(fd, "Failed to set session id: %s");
	}
	umask(0);
	chdir("/");
	close(0);
	close(1);
	close(2);
	if (open("/dev/null", O_RDWR) != 0)
	{
		// syslog(LOG_ERR, "Unable to open /dev/null: %s", strerror(errno));
		err_msg(fd, "Unable to open /dev/null: %s");
	}
	dup(0);
	dup(0);
	setsignals();
}

static void	detect_port(int sockfd, int fd)
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	FILE	*conf;
	int		port, pid;

	if (!(conf = fopen("/tmp/netsnifferd.conf", "w")))
		err_msg(fd, "Can't open .conf file: %s");
	if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
		err_msg(fd, "Unable to get socket name: %s");
	port = ntohs(sin.sin_port);
	pid = getpid();
	dprintf(fd, "Started and connected to port %d pid %u\n", port, pid);
	fprintf(conf, "%d\n%u", port, pid);
	fclose(conf);
}

static void	connector(int fd)
{
	int					sockfd, newsockfd;
	socklen_t			clilen;
	char				buffer[256];
	struct sockaddr_in	serv_addr, cli_addr;
	int					n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		err_msg(fd, "Failed to open socket: %s");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = 0;
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		err_msg(fd, "Failed to bind connection: %s");
	listen(sockfd, 5);
	detect_port(sockfd, fd);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			dprintf(fd, "Failed to accept connetction: %s", strerror(errno));
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n > 0)
		{
			dprintf(fd, "Here is the message: %s", buffer);			///////////
			n = write(newsockfd,"I got your message",18);
			if (n < 0)
				dprintf(fd, "Failed to write in socket: %s", strerror(errno));
		}
		else if (n < 0)
			dprintf(fd, "Failed to read from socket: %s", strerror(errno));
	}
	close(newsockfd);
	close(sockfd);
}

int			main(void)
{
	int fd;																////////

	// openlog("netsnifferd", LOG_PID, LOG_DAEMON);
	if ((fd = open("/tmp/netsnifferd.log", O_WRONLY | O_APPEND | O_CREAT,
		S_IRUSR | S_IWUSR)) == -1)///
		exit(1);													////////////
	if (access("/tmp/netsnifferd.conf", F_OK) != -1)
	{
		dprintf(fd, "Daemon already started");
		exit(1);
	}
	starter(fd);
	connector(fd);
	unlink("/tmp/netsnifferd.conf");
	close (fd);														////////////
	// closelog();
	return (0);
}
