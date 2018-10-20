#include "netsniffer_d.h"

static void	err_msg(const char *msg)
{
	dprintf(logfd, "[pid:%d:%lu] %s: %s\n",
		getpid(), time(0), msg, strerror(errno));
	exit(1);
}

static void	signal_handler(int signo)
{
	int status;

	if (signo == SIGTERM && getppid() == 1)
	{
		kill(0, SIGQUIT);
		wait(&status);
		close(confd);
		unlink("/tmp/netsnifferd.conf");
		dprintf(logfd, "[pid:%d:%lu] Shutting down\n", getpid(), time(0));
		exit(0);
	}
	else if (signo == SIGCHLD)
	{
		wait(&status);
		if (WIFEXITED(status))
			dprintf(logfd, "[pid:%d:%lu] Child process exited: %d\n",
				getpid(), time(0), WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status))
				dprintf(logfd, "[pid:%d:%lu] Child process terminate with\
 signal: %d\n", getpid(), time(0), WTERMSIG(status));
			else if (WCOREDUMP(status))
				dprintf(logfd, "[pid:%d:%lu] Child process terminate with\
 signal: %d\n", getpid(), time(0), WCOREDUMP(status));
		}
		else if (WIFSTOPPED(status))
			dprintf(logfd, "[pid:%d:%lu] Child process stopped: %d\n",
				getpid(), time(0), WSTOPSIG(status));
		child_run = 0;
	}
	else if (signo == SIGQUIT && getppid() > 1)
	{
		iface_connection(NULL, 1);
		exit(0);
	}
}

/* get port & pid values and write them into .conf file */
static void	detect_port(int sockfd)
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	int		port;

	if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
		err_msg("Unable to get socket name");
	port = ntohs(sin.sin_port);
	dprintf(logfd, "[pid:%d:%lu] Started and connected to port %d\n",
		getpid(), time(0), port);
	dprintf(confd, "%d\n", port);
	if (lockf(confd, F_LOCK, 0) == -1) /* */
		err_msg("Locking .conf file error");
}

static void	answer(int fd, const char *msg)
{
	if (write(fd, msg, strlen(msg)) < 0)
		dprintf(logfd, "[pid:%d:%lu] Socket writing error: %s",
			getpid(), time(0), strerror(errno));
}

static char *switch_iface(char *iface_name)
{
	static char iface[16];

	if (iface[0] == 0)
		strcpy(iface, DEFAULT_IFACE);
	else if (iface_name)
		strcpy(iface, iface_name);
	return (iface);
}

int		iface_connection(const char *iface, int mod)
{
	static int		sock_raw;
	struct ifreq	ifr;

	if (mod == 0)
	{
		if ((sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1)
			err_msg("Socket error");
		ifr.ifr_addr.sa_family = AF_INET;
		strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
		ioctl(sock_raw, SIOCGIFADDR, &ifr);
		bind(sock_raw, &ifr.ifr_addr, sizeof(ifr.ifr_addr));
		return (sock_raw);
	}
	else
		close(sock_raw);
	return (0);
}

static void launch_sniffer(void)
{
	pid_t			pid;
	char			*iface;
	int				saddr_size, data_size, sock_raw;
	struct sockaddr	saddr;
	char			buffer[65536];

	if (child_run)
		return ;
	pid = fork();
	iface = switch_iface(NULL);
	if (pid == -1)
		dprintf(logfd, "[pid:%d:%lu] Can't start sniffer\n", getpid(), time(0));
	else if (pid == 0)
	{
		dprintf(logfd, "[pid:%d:%lu] Sniffer startfed\n", getpid(), time(0));
		sock_raw = iface_connection(iface, 0);
		while (1)
		{
			saddr_size = sizeof(saddr);
			data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr,
				(socklen_t*)&saddr_size);
			if (data_size < 0)
				err_msg("Failed to count packets");
			struct ip *iph = (struct ip*)(buffer + sizeof(struct ip));
			dprintf(logfd, "%s\n", inet_ntoa(iph->ip_src));
		}
		exit (0);
	}
	else
		child_run = 1;
}

static void	connector(void)
{
	int					sockfd, newsockfd;
	socklen_t			clilen;
	char				buffer[17];
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
		{
			dprintf(logfd, "[pid:%d:%lu] Accepting connection error: %s\n",
				getpid(), time(0), strerror(errno));
			continue ;
		}
		bzero((void *)buffer, 17);
		n = read(newsockfd, buffer, 16);
		if (n > 0)
		{
			switch (buffer[0])
			{
				case '\01':
					launch_sniffer();
					answer(newsockfd, "\01");
					dprintf(logfd, "[pid:%d:%lu] Start sniffer\n",
						getpid(), time(0));
					break ;
				case '\02':
					kill(0, SIGQUIT);
					sleep(1);
					answer(newsockfd, "\01");
					dprintf(logfd, "[pid:%d:%lu] Stop sniffer\n",
						getpid(), time(0));
					break ;
				case '\03':
					switch_iface(buffer + 1);
					answer(newsockfd, "\01");
					dprintf(logfd, "[pid:%d:%lu] Switch iface\n",
						getpid(), time(0));
					break ;
				case '\04':
					answer(newsockfd, "\01");
					dprintf(logfd, "[pid:%d:%lu] Daemon shutting down\n",
						getpid(), time(0));
					kill(getpid(), SIGTERM);
					break ;
				default:
					answer(newsockfd, "\02");
					dprintf(logfd, "[pid:%d:%lu] Unknown data code received\n",
						getpid(), time(0));
			}
		}
		else if (n < 0)
			dprintf(logfd, "[pid:%d:%lu] Socket reading error: %s\n",
				getpid(), time(0), strerror(errno));
		close(newsockfd);
	}
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
		dprintf(logfd, "[pid:%d:%lu] Daemon already started\n",
			getpid(), time(0));
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
	// sigaddset(&set, SIGQUIT);
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
	if (sigaction(SIGQUIT, &act, NULL) == -1)
		err_msg("Can't set SIGCHLD signal");
	connector();
	return (0);
}
