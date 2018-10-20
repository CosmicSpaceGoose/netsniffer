#include "netsniffer_cli.h"

static int	daemon_is_running(void)
{
	int confd;

	if ((confd = open("/tmp/netsnifferd.conf", O_WRONLY)) != -1)
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
	if ((confd = open("/tmp/netsnifferd.conf", O_RDONLY)) < 0)
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

void	start_sniff(char *argv[])
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
				execlp("./netsnifferd", NULL);
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

void	stop_sniff(char *argv[])
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

void	show_ip(char *argv[])
{
	int running;

	argv++;
	if ((running = daemon_is_running()))
	{
		printf("%s", "netsniffercli: Stoping sniffer... ");
		transmit_data("\02");
	}
	/* do sum work */
	write(1, "!\n", 2);
	if (running)
	{
		printf("%s", "netsniffercli: Starting sniffer... ");
		transmit_data("\01");
	}
}

void	select_iface(char *argv[])
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

void	stat_iface(char *argv[])
{
	int running;

	argv++;
	if ((running = daemon_is_running()))
	{
		printf("%s", "netsniffercli: Stoping sniffer... ");
		transmit_data("\02");
	}
	/* do sum work */
	write(1, "!\n", 2);
	if (running)
	{
		printf("%s", "netsniffercli: Starting sniffer... ");
		transmit_data("\01");
	}
}

void	shuttdown_daemon(char *argv[])
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
