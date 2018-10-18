#include "netsniffer_d.h"

static void	err_msg(const char *msg) {
	dprintf(logfd, msg, strerror(errno));
	exit(1);
}

static void	signal_handler(int signo)
{
	if (signo == SIGTERM)
	{
		close(confd);
		unlink("/tmp/netsnifferd.conf");
		exit(0);
	}
}

static void	starter(void)
{
	sigset_t			set;
	struct sigaction	act;

	switch (fork())
	{
		case 0:
			break ;
		case -1:
			err_msg("Failed to fork process: %s");
			exit(1);
		default:
			exit(0);
	}
	if (setsid() < 0)
		err_msg("Failed to set session id: %s");
	umask(0);
	chdir("/");
	close(0);
	close(1);
	close(2);
	if (open("/dev/null", O_RDWR) != 0)
		err_msg("Unable to open /dev/null: %s");
	dup(0);
	dup(0);
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGCHLD);
	sigaddset(&set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &set, NULL);
	act.sa_handler = signal_handler;
	act.sa_mask = set;
	if (sigaction(SIGTERM, &act, NULL) == -1)
		err_msg("Can't set SIGTERM signal: ");
}

static void	detect_port(int sockfd)
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	int		port, pid;

	if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
		err_msg("Unable to get socket name: %s");
	port = ntohs(sin.sin_port);
	pid = getpid();
	dprintf(logfd, "Started and connected to port %d pid %u\n", port, pid);
	dprintf(confd, "%d\n%u", port, pid);
	if (lockf(confd, F_LOCK, 0) == -1)
		err_msg("Failed to lock .conf file: ");
}

static void	connector(void)
{
	int					sockfd, newsockfd;
	socklen_t			clilen;
	char				buffer[256];
	struct sockaddr_in	serv_addr, cli_addr;
	int					n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		err_msg("Failed to open socket: %s");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = 0;
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		err_msg("Failed to bind connection: %s");
	listen(sockfd, 5);
	detect_port(sockfd);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			dprintf(logfd, "Failed to accept connetction: %s", strerror(errno));
		bzero(buffer,256);
		n = read(newsockfd, buffer, 255);
		if (n > 0)
		{
			dprintf(logfd, "Here is the message: %s", buffer);
			n = write(newsockfd,"I got your message",18);
			if (n < 0)
				dprintf(logfd, "Failed to write in socket: %s", strerror(errno));
		}
		else if (n < 0)
			dprintf(logfd, "Failed to read from socket: %s", strerror(errno));
	}
	close(newsockfd);
	close(sockfd);
}

int			main(void)
{
	if ((logfd = open("/tmp/netsnifferd.log", O_WRONLY | O_APPEND | O_CREAT,
		S_IRUSR | S_IWUSR)) == -1)///
		exit(1);
	if ((confd = open("/tmp/netsnifferd.conf", O_WRONLY | O_CREAT,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) > -1
		&& lockf(confd, F_TEST, 0) == -1)
	{
		dprintf(logfd, "Daemon already started\n");
		exit(1);
	}
	else if (confd == -1)
		err_msg("Can't open .conf file: %s");
	starter();
	connector();
	return (0);
}
