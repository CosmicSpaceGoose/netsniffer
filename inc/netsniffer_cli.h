#ifndef NETSNIFFER_CLI_H
# define NETSNIFFER_CLI_H

# include <netinet/in.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <ifaddrs.h>
# include <netdb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

typedef struct	s_funptr {
	char	*name;
	void	(*func)(char *argv[]);
}				t_funptr;

extern const t_funptr g_commands[];

void	start_sniff(char *argv[]);
void	stop_sniff(char *argv[]);
void	show_ip(char *argv[]);
void	select_iface(char *argv[]);
void	stat_iface(char *argv[]);
void	restart_sniffer(char *argv[]);
void	shutdown_daemon(char *argv[]);

#endif
