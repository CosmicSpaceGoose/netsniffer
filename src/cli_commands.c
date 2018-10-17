#include "netsniffer_cli.h"

int		iface_checker(char *name)
{
	struct ifaddrs	*list;

	if (getifaddrs(&list))
	{
		perror("netsniffer: ");
		return (1);
	}
	while (list)
	{
		printf("%s\n", list->ifa_name);
		if (list->ifa_addr->sa_data[0] && !strcmp(name, list->ifa_name))
			return (0);
		list = list->ifa_next;
	}
	dprintf(2, "%s%s\n", "netsniffer: No such interface name: ", name);
	return (1);
}

void start_sniff(char *argv[])
{
	if (*argv && iface_checker(*argv))
		exit(1);
	// if (!launched())
	// 	launch_daemon(*argv);
	argv++;
}

void stop_sniff(char *argv[])
{
	argv++;
	/* code */
}

void show_ip(char *argv[])
{
	argv++;
	/* code */
}

void select_iface(char *argv[])
{
	argv++;
	/* code */
}

void stat_iface(char *argv[])
{
	argv++;
	/* code */
}
