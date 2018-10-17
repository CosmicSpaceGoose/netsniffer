# netsniffer
# Presented service collects statistic about network traffic

# Components:
	netsniffer_d - daemon<br>
	netsniffer_cli - command line interface (CLI) for daemon

# Daemon can be started independently or through the CLI.

# CLI usage:<br>
	netsniffercli [command_name [argument1 ...]]

# CLI commands:<br>
	start - packets sniffed from now on from default iface (eth0);<br>
	stop - packets are not sniffed;<br>
	show [ip] count - print numbers of packets recieved from ip address;<br>
	select iface [iface] - select interface for sniffing (ethN, wlanN, ...);<br>
	stat [iface] - show all collected statistics for particular interface,<br>
		if iface ommitted - for all interfaces;<br>
	--help [command name] - show usage information or specific command info;<br>
