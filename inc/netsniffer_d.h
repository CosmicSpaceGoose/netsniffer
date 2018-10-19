#ifndef NETSNIFFER_D_H
# define NETSNIFFER_D_H

# include <netinet/ip.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <syslog.h>
# include <time.h>
# include <unistd.h>

int		logfd, confd;

#endif
