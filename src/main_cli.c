#include "netsniffer_cli.h"

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
