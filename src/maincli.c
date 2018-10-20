#include "nsniffcli.h"

static int	daemon_is_running(void)
{
	int confd;

	if ((confd = open(CONF_FILE, O_WRONLY)) != -1)
	{
		if (lockf(confd, F_TEST, 0) == -1)
		{
			close(confd);
			return (1);
		}
		close(confd);
	}
	return (0);
}

static int	iface_checker(char *name, int mod)
{
	struct ifaddrs	*list, *copy;

	if (getifaddrs(&list))
	{
		perror("netsniffer: ");
		return (1);
	}
	copy = list;
	while (list)
	{
		if (!mod && list->ifa_addr->sa_data[0])
			printf("%d %s\n", list->ifa_addr->sa_data[0], list->ifa_name);
		if (mod && list->ifa_addr->sa_data[0] && !strcmp(name, list->ifa_name))
		{
			freeifaddrs(copy);
			return (1);
		}
		list = list->ifa_next;
	}
	if (mod)
		dprintf(2, "%s%s\n", "netsniffer: No such interface: ", name);
	freeifaddrs(copy);
	return (0);
}

static void	transmit_data(char data[])
{
	int				sockfd, i, confd, portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char			buffer[1024];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		perror("netsniffercli: Socket error");
	gethostname(buffer, 1023);
	if ((server = gethostbyname(buffer)) == NULL)
	{
		herror("netsniffercli: Host error");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	if ((confd = open(CONF_FILE, O_RDONLY)) < 0)
	{
		perror("netsniffercli: Opening .conf file error");
		exit(1);
	}
	if ((i = read(confd, buffer, 1023)) < 0)
	{
		perror("netsniffercli: Reading .conf file error");
		exit(1);
	}
	close(confd);
	buffer[i] = 0;
	sscanf(buffer, "%d", &portno);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		perror("netsniffercli: Connetction error");
		exit(1);
	}
	if (write(sockfd, data, strlen(data)) < 0)
		perror("netsniffercli: Socket reading error: ");
	if ((i = read(sockfd, buffer, 1)) < 0)
		perror("netsniffercli: Socket reading error: ");
	if (*buffer == 1)
		printf("Success\n");
	else
		printf("Fail\n");
	close(sockfd);
}

static void	start_sniff(char *argv[])
{
	int status;

	if (*argv)
		argv++;
	if (!daemon_is_running())
	{
		switch (fork())
		{
			case -1:
				perror("netsniffercli: Failed to run daemon: ");
				exit(1);
				break ;
			case 0:
				execlp("./nsniffd", "./nsniffd", NULL);
				perror("netsniffercli: Failed to launch daemon: ");
				exit(1);
				break ;
			default:
				wait(&status);
				if (status == 0)
					printf("%s", "netsniffercli: Daemon launched succesfully\n");
				exit(status);
		}
	}
	else
	{
		printf("%s", "netsniffercli: Starting sniffer... ");
		transmit_data("\01");
	}
}

static void	stop_sniff(char *argv[])
{
	if (*argv)
		argv++;
	if (daemon_is_running())
	{
		printf("%s", "netsniffercli: Stoping sniffer... ");
		transmit_data("\02");
	}
	else
		dprintf(2, "%s", "netsniffercli: Daemon isn't launched\n");
}

static void	show_ip(char *argv[])
{
	argv++;
	/* do sum work */
	write(1, "!\n", 2);
}

static void	select_iface(char *argv[])
{
	char	data[17];

	if (!*argv)
		iface_checker(NULL, 0);
	else if (!iface_checker(*argv, 1))
		exit(1);
	else if (daemon_is_running())
	{
		data[0] = '\03';
		strcpy(data + 1, *argv);
		printf("%s", "netsniffercli: Selecting interface... ");
		transmit_data(data);
	}
	else
		dprintf(2, "%s", "netsniffercli: Daemon isn't launched\n");
}

static void	stat_iface(char *argv[])
{
	int fd;
	char buffer[sizeof(t_data)];
	t_data	entry;

	argv++;
	if ((fd = open(DATA_FILE, O_RDONLY)) == -1)
	{
		perror("Can't read data file");
		exit (1);
	}
	while (read(fd, buffer, sizeof(t_data)))
	{
		entry = *(t_data *)buffer;
		printf("%u.%u.%u.%u %s %llu\n", buffer[0], buffer[1], buffer[2],
			buffer[3], entry.iface, entry.count);
	}
	close(fd);
}

static void	shuttdown_daemon(char *argv[])
{
	argv++;
	if (daemon_is_running())
	{
		printf("%s", "netsniffercli: Shutting down... ");
		transmit_data("\04");
	}
	else
		dprintf(2, "%s", "netsniffercli: Daemon isn't launched\n");
}

const char *help_command[] = {
	"start - packets sniffed from now on from default interface (eth0).\n",
	"stop - stop sniff packets from recent interface.\n",
	"show [ip] - print numbers of packets received from ip address.\n",
	"select [iface] - select interface for sniffing (ethN, wlanN, ...),\
\n	if interface omitted - list available interfaces names",
	"stat [iface] - show all collected statistics for particular interface,\
\n 	if interface omitted - for all interfaces.\n",
	"--help [command name] - show usage or specific command info.\n",
	NULL
};

static void	usage(char *argv[])
{
	int i = 0;

	if (!argv || !*argv)
	{
		dprintf(2, "%s", "usage: netstat_cli [command_name [argument1 ...]]\n\
available commands: start stop show select stat shuttdown --help\ntype:\
 \"netstat_cli --help [command_name]\" for info about specific command.\n");
	}
	else
	{
		while (g_commands[i].name)
		{
			if (!strcmp(*argv, g_commands[i].name))
			{
				printf("%s", help_command[i]);
				return ;
			}
			i++;
		}
		dprintf(2, "%s: %s\n", "netsniffer_cli: Unknown command name", *argv);
	}
}

const t_funptr g_commands[] = {
	{"start", start_sniff},
	{"stop", stop_sniff},
	{"show", show_ip},
	{"select", select_iface},
	{"stat", stat_iface},
	{"shuttdown", shuttdown_daemon},
	{"--help", usage},
	{NULL, NULL}
};

int			main(int argc, char *argv[])
{
	int i = 0;

	if (argc == 1 || !strcmp(argv[1], "--"))
		usage(NULL);
	else
	{
		argv++;
		while (g_commands[i].name)
		{
			if (!strcmp(*argv, g_commands[i].name))
			{
				g_commands[i].func(argv + 1);
				return (0);
			}
			i++;
		}
		dprintf(2, "%s: %s\n", "netsniffer_cli: Unknown command name", *argv);
	}
}
