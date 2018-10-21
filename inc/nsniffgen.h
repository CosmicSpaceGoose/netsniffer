#ifndef NSNIFFGEN_H
# define NSNIFFGEN_H

/* general header */
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
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

typedef struct	s_data {
	uint64_t	addr;
	char		iface[16];
	uint64_t	count;
}				t_data;

#endif	/* NSNIFFGEN_H */
