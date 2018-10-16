# netstat
#Presented service collects statistic about network traffic

#Components:
	netstat_d - daemon
	netstat_cli - command line interface (CLI) for deamon

#Daemon can be started independently or through the CLI;

#CLI commands:
	start - packets sniffed from now on from default iface (eth0);
	stop - packets are not sniffed;
	show [ip] count - print numbers of packets recieved from ip address;
	select iface [iface] - select interface for sniffing (ethN, wlanN, ...);
	stat [iface] - show all collected statistics for particular interface,
		if iface ommitted - for all interfaces;
	--help - show usage information;
