#ifndef NSNIFFCLI_H
# define NSNIFFCLI_H

/* command line interface header */
# include <sys/types.h>
# include <ifaddrs.h>
# include <netdb.h>
# include "nsniffgen.h"

typedef struct	s_funptr {
	char	*name;
	void	(*func)(char *argv[]);
}				t_funptr;

extern const t_funptr g_commands[];

#endif /* NSNIFFCLI_H */
