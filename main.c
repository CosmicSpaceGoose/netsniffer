
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
# include <net/if.h>
# include <netinet/ip.h>
# include <sys/ioctl.h>
# include <sys/stat.h>
# include <errno.h>
# include <signal.h>
# include <time.h>
# if defined __APPLE__ || defined __MACH__
#  define DEFAULT_IFACE "en0"
# elif defined __linux__
#  define DEFAULT_IFACE "eth0"
# endif
# define DATA_DIR "./nsniffd"
# define CONF_FILE "./nsniffd.conf"
# define LOG_FILE "./nsniffd.log"
# define DATA_FILE "./nsniffd.data"
# define ADD 0
# define INC 1
# define SAVE 2
typedef struct	s_data {
	uint64_t	addr;
	char		iface[16];
	uint64_t	count;
}				t_data;

extern	t_data	*dptr;
int		logfd, confd, child_run;

static void	err_msg(const char *msg)
{
	dprintf(logfd, "[pid:%d:%lu] %s: %s\n",
		getpid(), time(0), msg, strerror(errno));
	exit(1);
}
int		iface_connection(const char *iface, int mod)
{
	static int		sock_raw;
	struct ifreq	ifr;

	if (mod == 0)
	{
		if ((sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1)
			err_msg("Socket error");
		ifr.ifr_addr.sa_family = AF_INET;
		strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
		ioctl(sock_raw, SIOCGIFADDR, &ifr);
		bind(sock_raw, &ifr.ifr_addr, sizeof(ifr.ifr_addr));
		return (sock_raw);
	}
	else
		close(sock_raw);
	return (0);
}

/*
**	function wrapper for data
*/
void	bucket(char *data, int mod)
{
	static t_data	*dptr;
	static size_t	last, total;
	size_t			i, max, min;
	int				fd;
	t_data			entry;

	/*
	**	add entry at the end of array or add 1st entry
	*/
	if (mod == ADD || (mod == INC && dptr == NULL))
	{
		if (last == total)
		{
			dptr = (t_data *)realloc((void *)dptr, (total + 100) * sizeof(t_data));
			total += 100;
		}
		memcpy((void *)dptr + last, (void *)data, sizeof(t_data));
		last++;
	}
	/*
	**	increas number of packets received from ip or add new entry
	*/
	else if (mod == INC)
	{
		entry = *(t_data *)data;
		min = 0;
		max = last - 1;
		while (min <= max)
		{
			i = (min + max) / 2;
			if (entry.addr > dptr[i].addr || max == 0)
				min = i + 1;
			else if (entry.addr == dptr[i].addr)
			{
				while (i != 0 && dptr[i - 1].addr == entry.addr)
					i--;
				while (i < last && dptr[i].addr == entry.addr)
				{
					if (!strcmp(dptr[i].iface, entry.iface))
					{
						dptr[i].count++;
						return ;
					}
					i++;
				}
				max = i - 1;
				min = i;
				break ;
			}
			else
				max = i - 1;
		}
		/*
		**	insert new entry if it not present
		*/
		if (min > max)
		{
			if (last == total)
			{
				dptr = (t_data *)realloc((void *)dptr, (total + 100)
					* sizeof(t_data));
				total += 100;
			}
			if (entry.addr < dptr[0].addr)
			{
				memmove((void *)(dptr + 1), (void *)dptr, sizeof(t_data));
				memcpy((void *)dptr, (void *)data, sizeof(t_data));
			}
			else if (entry.addr > dptr[last - 1].addr)
				memcpy((void *)(dptr + last), (void *)data, sizeof(t_data));
			else
			{
				memmove((void *)(dptr + max + 2), (void *)(dptr + max + 1),
					last - 1 - max);
				memcpy((void *)(dptr + max + 1), (void *)data, sizeof(t_data));
			}
			last++;
		}
	}
	/*
	**	save all data in file
	*/
	else if (mod == SAVE)
	{
		i = 0;
		if ((fd = open(DATA_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1)
			err_msg("Can't open data file for writing");
		while (i < last)
		{
			write(fd, (void *)(dptr + i), sizeof(t_data));
			i++;
		}
		close(fd);
		free(dptr);
	}
}

static void	load_data(void)
{
	int		fd;
	char	data[sizeof(t_data)];

	if (access(DATA_FILE, F_OK) == -1)
		return ;
	if ((fd = open(DATA_FILE, O_RDONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1)
		err_msg("Can't open data file for reading");
	while (read(fd, data, sizeof(t_data)))
		bucket(data, ADD);
	close(fd);
}

int main()
{
	pid_t			pid;
	char			*iface;
	int				saddr_size, data_size, sock_raw, i;
	struct sockaddr	saddr;
	char			buffer[65536];
	struct ip		*iph;
	t_data			entry;

	dprintf(logfd, "[pid:%d:%lu] Sniffer startfed\n", getpid(), time(0));
	sock_raw = iface_connection(iface, 0);
	load_data();
	i = 10;
	while (i)
	{
		saddr_size = sizeof(saddr);
		data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr,
			(socklen_t*)&saddr_size);
		if (data_size < 0)
			err_msg("Failed to count packets");
		iph = (struct ip*)(buffer + sizeof(struct ip));
		bzero((void *)&entry, sizeof(t_data));
		entry.addr = (uint32_t)iph->ip_src.s_addr;
		strcpy(entry.iface, iface);
		bucket((char *)&entry, INC);
		dprintf(logfd, ">%u\n", iph->ip_src.s_addr);
	}
	bucket(NULL, SAVE);
	iface_connection(NULL, 1);
	return (0);
}
