#ifndef NSNIFFD_H
# define NSNIFFD_H

/* daemon header */
# include <net/if.h>
# include <netinet/ip.h>
# include <sys/ioctl.h>
# include <sys/stat.h>
# include <errno.h>
# include <signal.h>
# include <time.h>
# include "nsniffgen.h"

# define ADD 0
# define INC 1
# define SAVE 2

extern	t_data	*dptr;
int		logfd, confd, child_run;

int		iface_connection(const char *iface, int mod);
void	bucket(char *data, int mod);

#endif /* NSNIFFD_H */
