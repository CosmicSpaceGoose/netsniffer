#ifndef NSNIFFCLI_H
# define NSNIFFCLI_H

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

# if defined __APPLE__ || defined __MACH__
#  define CONF_FILE "/tmp/nsniffd/nsniffd.conf"
#  define DATA_FILE "/tmp/nsniffd/nsniffd.data"
# elif defined __linux__
#  define CONF_FILE "/var/nsniffd/nsniffd.conf"
#  define DATA_FILE "/var/nsniffd/nsniffd.data"
# endif

typedef struct	s_data {
	uint32_t	addr;
	char		iface[16];
	uint64_t	count;
}				t_data;

typedef struct	s_funptr {
	char	*name;
	void	(*func)(char *argv[]);
}				t_funptr;

extern const t_funptr g_commands[];

#endif
