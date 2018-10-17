#include "netsniffer_d.h"

int		main(void)
{
	int sck;

	if (argc > 1)
		daemon_starter("");
	else
	{
		daemon_starter();
	}
	return (0);
}
