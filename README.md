# netstat
#Presented service collects statistic about network traffic

#Components:
	netstat_d - daemon
	netstat_cli - command line interface (CLI) for deamon

#Daemon can be started independently or through the CLI;

#CLI commands:
	start - packets sniffed from now on from default iface (eth0);<br>
	stop - packets are not sniffed;<br>
	show [ip] count - print numbers of packets recieved from ip address;<br>
	select iface [iface] - select interface for sniffing (ethN, wlanN, ...);<br>
	stat [iface] - show all collected statistics for particular interface,<br>
		if iface ommitted - for all interfaces;<br>
	--help - show usage information;<br>
