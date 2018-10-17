# netsniffer
## Presented service collects statistic about network traffic

## Components:
	netsniffer_d - daemon
	netsniffer_cli - command line interface (CLI) for daemon

	Daemon can be started independently or through the CLI.

## CLI usage:
	netsniffercli [command_name [argument1 ...]]

## CLI commands:
	start - packets sniffed from now on from default iface (eth0);
	stop - packets are not sniffed;
	show [ip] count - print numbers of packets recieved from ip address;
	select iface [iface] - select interface for sniffing (ethN, wlanN, ...);
	stat [iface] - show all collected statistics for particular interface,
		if iface ommitted - for all interfaces;
	--help [command name] - show usage information or specific command info;
