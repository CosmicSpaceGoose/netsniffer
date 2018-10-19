#include "netsniffer_d.h"

static void	err_msg(const char *msg)
{
	dprintf(logfd, "[%lu] %s: %s", time(0), msg, strerror(errno));
	exit(1);
}

static void	signal_handler(int signo)
{
	int status;

	if (signo == SIGTERM)
	{
		kill(0, SIGUSR1);
		close(confd);
		unlink("/tmp/netsnifferd.conf");
		dprintf(logfd, "[%lu] Shutting down\n", time(0));
		exit(0);
	}
	else if (signo == SIGCHLD)
	{
		wait(&status);
		if (WIFEXITED(status))
			dprintf(logfd, "[%lu] Child process exited: %d\n", time(0),
				WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status))
				dprintf(logfd, "[%lu] Child process terminate with signal: %d\n"
					, time(0), WTERMSIG(status));
			else if (WCOREDUMP(status))
				dprintf(logfd, "[%lu] Child process terminate with signal: %d\n"
					, time(0), WCOREDUMP(status));
		}
		else if (WIFSTOPPED(status))
			dprintf(logfd, "[%lu] Child process stopped: %d\n", time(0),
				WSTOPSIG(status));
	}
}

static void	detect_port(int sockfd)
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	int		port, pid;

	if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
		err_msg("Unable to get socket name");
	port = ntohs(sin.sin_port);
	pid = getpid();
	dprintf(logfd, "[%lu] Started and connected to port %d pid %u\n", time(0),
		port, pid);
	dprintf(confd, "%d\n%u", port, pid);
	if (lockf(confd, F_LOCK, 0) == -1)
		err_msg("Locking .conf file error");
}

static void	answer(int fd, const char *msg)
{
	if (write(fd, msg, strlen(msg)) < 0)
		dprintf(logfd, "[%lu] Socket writing error: %s", time(0),
			strerror(errno));
}

static void launch_sniffer(void)
{
	/* code */
}

static void stop_sniffer(void)
{
	/* code */
}

static void switch_iface(int iface_no)
{
	iface_no++;
}

static void	connector(void)
{
	int					sockfd, newsockfd;
	socklen_t			clilen;
	char				buffer[2];
	struct sockaddr_in	serv_addr, cli_addr;
	int					n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		err_msg("Socket opening error");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = 0;
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		err_msg("Binding connection error");
	listen(sockfd, 5);
	detect_port(sockfd);
	clilen = sizeof(cli_addr);
	launch_sniffer();
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			dprintf(logfd, "[%lu] Accepting connection error: %s", time(0),
				strerror(errno));
		buffer[0] = 0;
		buffer[1] = 0;
		n = read(newsockfd, buffer, 2);
		if (n > 0)
		{
			dprintf(logfd, "Recieve %u %u\n", buffer[0], buffer[1]);
			switch (buffer[0])
			{
				case '\01':
					launch_sniffer();
					answer(newsockfd, "Launch sniffer");
					break ;
				case '\02':
					stop_sniffer();
					answer(newsockfd, "Stop sniffer");
					break ;
				case '\03':
					switch_iface((int)buffer[1]);
					answer(newsockfd, "Switch iface");
					break ;
				case '\04':
					answer(newsockfd, "Daemon shutting down");
					kill(getpid(), SIGTERM);
					break ;
				default:
					answer(newsockfd, "Unknown data code received");
			}
		}
		else if (n < 0)
			dprintf(logfd, "[%lu] Socket reading error: %s", time(0),
				strerror(errno));
	}
	close(newsockfd);
	close(sockfd);
}

int			main(void)
{
	sigset_t			set;
	struct sigaction	act;

	if ((logfd = open("/tmp/netsnifferd.log", O_WRONLY | O_APPEND | O_CREAT,
		S_IRUSR | S_IWUSR)) == -1)
		exit(1);
	if ((confd = open("/tmp/netsnifferd.conf", O_WRONLY | O_CREAT,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) > -1
		&& lockf(confd, F_TEST, 0) == -1)
	{
		dprintf(logfd, "[%lu] Daemon already started\n", time(0));
		exit(1);
	}
	else if (confd == -1)
		err_msg("Can't open .conf file");
	switch (fork())
	{
		case 0:
			break ;
		case -1:
			err_msg("Failed to fork process");
			exit(1);
		default:
			exit(0);
	}
	if (setsid() < 0)
		err_msg("Failed to set session id");
	umask(0);
	chdir("/");
	close(0);
	close(1);
	close(2);
	if (open("/dev/null", O_RDWR) != 0)
		err_msg("Unable to open /dev/null");
	dup(0);
	dup(0);
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &set, NULL);
	act.sa_handler = signal_handler;
	act.sa_mask = set;
	// act.sa_flags = NO_CLDWAIT;
	if (sigaction(SIGTERM, &act, NULL) == -1)
		err_msg("Can't set SIGTERM signal");
	if (sigaction(SIGCHLD, &act, NULL) == -1)
		err_msg("Can't set SIGCHLD signal");
	connector();
	return (0);
}
