#include "netsniffer_cli.h"

int		iface_checker(char *name, int mod)
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
			return (0);
		list = list->ifa_next;
	}
	if (mod)
		dprintf(2, "%s%s\n", "netsniffer: No such interface: ", name);
	return (1);
}

void start_sniff(char *argv[])
{
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
	if (!*argv)
		iface_checker(NULL, 0);
	else if (iface_checker(*argv, 1))
		exit(1);
	else
		;// switch iface
}

void stat_iface(char *argv[])
{
	argv++;
	/* code */
}
