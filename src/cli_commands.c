#include "netsniffer_cli.h"

static int	iface_checker(char *name, int mod)
{
	struct ifaddrs	*list;

	if (getifaddrs(&list))
	{
		perror("netsniffer: ");
		return (1);
	}
	while (list)
	{
		if (!mod)// && list->ifa_addr->sa_data[0])
			printf("%d %s\n", list->ifa_addr->sa_data[0], list->ifa_name);
		if (mod && list->ifa_addr->sa_data[0] && !strcmp(name, list->ifa_name))
		{
			freeifaddrs(list);
			return (0);
		}
		list = list->ifa_next;
	}
	if (mod)
		dprintf(2, "%s%s\n", "netsniffer: No such interface: ", name);
	freeifaddrs(list);
	return (1);
}

void	start_sniff(char *argv[])
{
	int st;

	if (*argv)
		argv++;
	if (access("/tmp/netsnifferd.conf", F_OK) == -1)
	{
		switch (fork()) {
			case -1:
				perror("netsniffercli: Failed to run daemon: ");
				exit(1);
			case 0:
				execlp("netsnifferd", (char *)0);
				perror("netsniffercli: failed to launch daemon: ");
			default:
				wait(&st);
				exit(st);
		}
	}
	// make connection & deliever data
}

void	stop_sniff(char *argv[])
{
	if (*argv)
		argv++;
	if (access("/tmp/netsnifferd.conf", F_OK) == -1)
	{
		dprintf(2, "%s", "netsniffercli: daemon isn't running\n");
		exit(0);
	}
	// make connection & deliever data
}

void	show_ip(char *argv[])
{
	argv++;
	/* code */
}

void	select_iface(char *argv[])
{
	if (!*argv)
		iface_checker(NULL, 0);
	else if (iface_checker(*argv, 1))
		exit(1);
	else
		;// switch iface
}

void	stat_iface(char *argv[])
{
	argv++;
	/* code */
}

void	restart_sniffer(char *argv[])
{
	argv++;
	/* code */
}
