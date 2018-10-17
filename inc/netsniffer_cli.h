#ifndef NETSNIFFER_CLI_H
# define NETSNIFFER_CLI_H

# include <sys/types.h>
# include <sys/socket.h>
# include <ifaddrs.h>
# include <stdio.h>
# include <netinet/in.h>
# include <string.h>
# include <stdlib.h>

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

#endif
