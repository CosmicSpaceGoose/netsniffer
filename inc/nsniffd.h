#ifndef NSNIFFD_H
# define NSNIFFD_H

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
#  define DATA_DIR "/tmp/nsniffd"
#  define CONF_FILE "/tmp/nsniffd/nsniffd.conf"
#  define LOG_FILE "/tmp/nsniffd/nsniffd.log"
#  define DATA_FILE "/tmp/nsniffd/nsniffd.data"
# elif defined __linux__
#  define DEFAULT_IFACE "eth0"
#  define DATA_DIR "/var/nsniffd"
#  define CONF_FILE "/var/nsniffd/nsniffd.conf"
#  define LOG_FILE "/var/nsniffd/nsniffd.log"
#  define DATA_FILE "/var/nsniffd/nsniffd.data"
# endif
# define ADD 0
# define INC 1
# define SAVE 2

typedef struct	s_data {
	uint32_t	addr;
	char		iface[16];
	uint64_t	count;
}				t_data;

extern	t_data	*dptr;
int		logfd, confd, child_run;

int		iface_connection(const char *iface, int mod);
void	bucket(char *data, int mod);

#endif
