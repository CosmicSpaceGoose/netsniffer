# nsniffd
## Presented service collects statistic about network traffic

## Components:
	nsniffd - daemon
	nsniffcli - command line interface (CLI) for daemon

	Daemon can be started independently or through the CLI.

## CLI usage:
	./nsniffcli [command_name [argument1 ...]]

## CLI commands:
	start - packets sniffed from now on from default interface (eth0);
	stop - packets are not sniffed;
	show [ip] count - print numbers of packets received from ip address;
	select [iface] - select interface for sniffing (ethN, wlanN, ...),
		if interface omitted - list available interfaces names;
	stat [iface] - show all collected statistics for particular interface,
		if interface omitted - for all interfaces;
	--help [command name] - show usage information or specific command info;
