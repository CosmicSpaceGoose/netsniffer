#ifndef NETSNIFFER_D_H
# define NETSNIFFER_D_H

# include <arpa/inet.h>
# include <net/if.h>
# include <netinet/ip.h>
# include <netinet/in.h>
# include <sys/ioctl.h>
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

# if defined __APPLE__ || defined __MACH__
#  define DEFAULT_IFACE "en0"
#  define iphdr ip
# elif defined __linux__
#  define DEFAULT_IFACE "eth0"
# endif

int		logfd, confd, child_run;

int		iface_connection(const char *iface, int mod);

#endif
