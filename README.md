# nsniffd
## Presented service collects statistic about network traffic

## Components:
	nsniffd - daemon
	nsniffcli - command line interface (CLI) for daemon

## Daemon usage:
	./nsniffd

	Daemon can be started independently or through the CLI. After launch,
	daemon start sniffer process, which sniff from default interface (eth0 on
	Linux or en0 on Mac Os X). At a time, there can be only one daemon process
	and one sniffer process. All data save in _nsniffd/_ folder. If folder not
	exist, daemon create it. If folder isn't reachable, daemon abort execution.
	In data folder may be .log file for logging, .conf file for controlling
	daemon instance and .data file for all collected data if it is.

## CLI usage:
	./nsniffcli [command_name [argument1 ...]]

### CLI commands:
	start - packets sniffed from now on from default interface (eth0);
	stop - packets are not sniffed;
	show [ip] count - print numbers of packets received from ip address;
	select [iface] - select interface for sniffing (ethN, wlanN, ...),
		if interface omitted - list available interfaces names;
	stat [iface] - show all collected statistics for particular interface,
		if interface omitted - for all interfaces;
	shuttdown - stop daemon work and exit,
	--help [command name] - show usage information or specific command info;

	*Note:* When entering _show_ or _select_ or _stat_ commands sniffer process
	isn't stopped, it continue to retrieve information and if you want see
	last actual statistic or switch on another interface - type _stop_,
	_chosen command_ and _start_ for restarting sniffer.
