#include "nsniffd.h"

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
		unlink(CONF_FILE);
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
		bucket(NULL, SAVE);
		iface_connection(NULL, 1);
		exit(0);
	}
}

/*
**	get port value and write it into .conf file, then lock file
*/
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

/*
**		open or close ip connection
*/
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

/*
**	function wrapper for data
*/
void	bucket(char *data, int mod)
{
	static t_data	*dptr;
	static size_t	last, total;
	size_t			i, max, min;
	int				fd;
	t_data			entry;

	/*
	**	add entry at the end of array or add 1st entry
	*/
	if (mod == ADD || (mod == INC && dptr == NULL))
	{
		if (last == total)
		{
			dptr = (t_data *)realloc((void *)dptr, (total + 100) * sizeof(t_data));
			total += 100;
		}
		memcpy((void *)dptr + last, (void *)data, sizeof(t_data));
		last++;
	}
	/*
	**	increas number of packets received from ip or add new entry
	*/
	else if (mod == INC)
	{
		entry = *(t_data *)data;
		min = 0;
		max = last - 1;
		while (min <= max)
		{
			i = (min + max) / 2;
			if (entry.addr > dptr[i].addr || max == 0)
				min = i + 1;
			else if (entry.addr == dptr[i].addr)
			{
				while (i != 0 && dptr[i - 1].addr == entry.addr)
					i--;
				while (i < last && dptr[i].addr == entry.addr)
				{
					if (!strcmp(dptr[i].iface, entry.iface))
					{
						dptr[i].count++;
						return ;
					}
					i++;
				}
				max = i - 1;
				min = i;
				break ;
			}
			else
				max = i - 1;
		}
		/*
		**	insert new entry if it not present
		*/
		if (min > max)
		{
			if (last == total)
			{
				dptr = (t_data *)realloc((void *)dptr, (total + 100)
					* sizeof(t_data));
				total += 100;
			}
			if (entry.addr < dptr[0].addr)
			{
				memmove((void *)(dptr + 1), (void *)dptr, sizeof(t_data));
				memcpy((void *)dptr, (void *)data, sizeof(t_data));
			}
			else if (entry.addr > dptr[last - 1].addr)
				memcpy((void *)(dptr + last), (void *)data, sizeof(t_data));
			else
			{
				memmove((void *)(dptr + max + 2), (void *)(dptr + max + 1),
					last - 1 - max);
				memcpy((void *)(dptr + max + 1), (void *)data, sizeof(t_data));
			}
			last++;
		}
	}
	/*
	**	save all data in file
	*/
	else if (mod == SAVE)
	{
		i = 0;
		if ((fd = open(DATA_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1)
			err_msg("Can't open data file for writing");
		while (i < last)
		{
			write(fd, (void *)(dptr + i), sizeof(t_data));
			i++;
		}
		close(fd);
		free(dptr);
	}
}

static void	load_data(void)
{
	int		fd;
	char	data[sizeof(t_data)];

	if (access(DATA_FILE, F_OK) == -1)
		return ;
	if ((fd = open(DATA_FILE, O_RDONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1)
		err_msg("Can't open data file for reading");
	while (read(fd, data, sizeof(t_data)))
		bucket(data, ADD);
	close(fd);
}

static void launch_sniffer(void)
{
	pid_t			pid;
	char			*iface;
	int				saddr_size, data_size, sock_raw;
	struct sockaddr	saddr;
	char			buffer[65536];
	struct ip		*iph;
	t_data			entry;

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
		load_data();
		while (1)
		{
			saddr_size = sizeof(saddr);
			data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr,
				(socklen_t*)&saddr_size);
			if (data_size < 0)
				err_msg("Failed to count packets");
			iph = (struct ip*)(buffer + sizeof(struct ip));
			bzero((void *)&entry, sizeof(t_data));
			entry.addr = (uint32_t)iph->ip_src.s_addr;
			strcpy(entry.iface, iface);
			bucket((char *)&entry, INC);
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
			if (buffer[0] == '\01')
			{
				launch_sniffer();
				answer(newsockfd, "\01");
				dprintf(logfd, "[pid:%d:%lu] Start sniffer\n", getpid(), time(0));
			}
			else if (buffer[0] == '\02')
			{
				kill(0, SIGQUIT);
				sleep(1);
				answer(newsockfd, "\01");
				dprintf(logfd, "[pid:%d:%lu] Stop sniffer\n", getpid(), time(0));
			}
			else if (buffer[0] == '\03')
			{
				switch_iface(buffer + 1);
				answer(newsockfd, "\01");
				dprintf(logfd, "[pid:%d:%lu] Switch iface\n", getpid(), time(0));
			}
			else if (buffer[0] == '\04')
			{
				answer(newsockfd, "\01");
				dprintf(logfd, "[pid:%d:%lu] Daemon shuting down\n",
					getpid(), time(0));
				kill(getpid(), SIGTERM);
			}
			else
			{
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
	struct stat			dir;

	if (stat(DATA_DIR, &dir) == -1 && mkdir(DATA_DIR, 0700))
		perror("Can't find data directory");
	if ((logfd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT,
		S_IRUSR | S_IWUSR)) == -1)
		exit(1);
	if ((confd = open(CONF_FILE, O_WRONLY | O_CREAT,
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
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &set, NULL);
	act.sa_handler = signal_handler;
	act.sa_mask = set;
	if (sigaction(SIGTERM, &act, NULL) == -1)
		err_msg("Can't set SIGTERM signal");
	if (sigaction(SIGCHLD, &act, NULL) == -1)
		err_msg("Can't set SIGCHLD signal");
	if (sigaction(SIGQUIT, &act, NULL) == -1)
		err_msg("Can't set SIGCHLD signal");
	connector();
	return (0);
}
