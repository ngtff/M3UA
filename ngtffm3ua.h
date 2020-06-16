#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <poll.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <error.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <resolv.h>

#ifndef LIB_NGTFF_M3UA_H
#define LIB_NGTFF_M3UA_H

typedef struct __ngtff_ss7_sccp_msg
{
	char * buff;
	int len;
	uint8_t MesageType;
	uint8_t MesageClass;
	uint8_t MesageHandling;
	uint8_t HopCounter;
	uint8_t pointerToFMP;
	uint8_t pointerToSMP;
	uint8_t pointerToTMP;
	uint8_t pointerToOP;
	uint8_t * calledPartyAddress;
	uint8_t * callingPartyAddress;
	uint8_t * sccpData;
	uint8_t * optData;
	
} SI_NGTFF_SCCPMsg;

void __ngtff_ss7__init();
void __ngtff_ss7__load_stackconfig( char * filename);

typedef void (*fp_ngtff_ss7__send_sccp)( uint32_t aspId, uint32_t opc, uint32_t dpc, uint8_t ni, uint8_t mp, uint8_t sls, SI_NGTFF_SCCPMsg * sccpMsg);
void __ngtff_ss7__send_sccp( uint32_t aspId, uint32_t opc, uint32_t dpc, uint8_t ni, uint8_t mp, uint8_t sls, SI_NGTFF_SCCPMsg * sccpMsg);
void __ngtff_ss7__on_recv_sccp_msg( fp_ngtff_ss7__send_sccp fc);

#endif

