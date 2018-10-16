#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int sck;

	if (argc > 1)
		write(1, argv[1], 3);
	else
	{
		sck = socket(AF_INET, SOCK_RAW, )
	}
	return (0);
}
