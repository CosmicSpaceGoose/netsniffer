#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

// int main(void)
// {
//    int    iSocket = -1;
//    struct if_nameindex* pIndex = 0;
//    struct if_nameindex* pIndex2 = 0;
//
//    if ((iSocket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
//    {
//       perror("socket");
//       return -1;
//    }
//    pIndex = pIndex2 = if_nameindex();
//    while ((pIndex != NULL) && (pIndex->if_name != NULL))
//    {
//       struct ifreq req;
//       printf("%d: %s\t", pIndex->if_index, pIndex->if_name);
//       strncpy(req.ifr_name, pIndex->if_name, IFNAMSIZ);
// 	  req.ifr_flags = 0;
//       if (ioctl(iSocket, SIOCGIFADDR, &req) < 0)
//       {
//          if (errno == EADDRNOTAVAIL)
//          {
//             printf("\tN/A\n");
//             ++pIndex;
//             continue;
//          }
//          perror("ioctl");
//          close(iSocket);
//          return -1;
//       }
//       printf("\t %u\n", ((struct sockaddr_in*)&req.ifr_addr)->sin_addr.s_addr);
//       ++pIndex;
//    }
//    if_freenameindex(pIndex2);
//    close(iSocket);
//    return 0;
// }
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<netinet/if_ether.h>  //For ETH_P_ALL
#include<net/ethernet.h>  //For ether_header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
int main(int argc, char const *argv[]) {
	printf("%d\n", SO_BINDTODEVICE);
	return 0;
}
