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

#define NGTFF_MESSAGE_TYPE__UNITDATA				9
#define NGTFF_MESSAGE_TYPE__XUNITDATA				17



typedef struct __ngtff_sai
{
	uint8_t pci: 1;
	uint8_t ssni : 1;
	uint8_t gti : 4;			
	uint8_t ri : 1;
	uint8_t rnu : 1;
} NGTFFSAI;

typedef struct __ngtff_sgt
{
	uint8_t oe;
	uint8_t nai;
	uint8_t np:4;
	uint8_t es:4;
	uint8_t tt;
	uint8_t d[20];
	uint8_t dl;
} NGTFFSGT;

typedef struct __ngtff_sa
{
	NGTFFSAI sai;
	uint16_t spc;
	uint8_t ssn;
	
	NGTFFSGT gt;
} NGTFFSA;

typedef struct __ngtff_sm
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
	uint8_t calledPartyAddresslen;
	uint8_t * callingPartyAddress;
	uint8_t callingPartyAddresslen;
	uint8_t * sccpData;
	uint8_t sccpDatalen;
	uint8_t * optData;
	uint8_t optDatalen;
	
} NGTFF_SM;

typedef struct ngtff_error
{
	int ErrNo;
	u_char Msg[249];
} NGTFF_Error;

uint8_t * __ngtff_ss7__malloc( size_t sz);
void __ngtff_ss7__free( void * m);

void __ngtff_ss7__init();
void __ngtff_ss7__load_stackconfig( char * filename);

void __ngtff_ss7__set_calledPartyAddressBuffer( NGTFF_SM * sm, uint8_t * buff, int len);
void __ngtff_ss7__encode_calledPartyAddress( NGTFF_SM * sm, NGTFFSA * cpad);

void __ngtff_ss7__set_callingPartyAddressBuffer( NGTFF_SM * sm, uint8_t * buff, int len);
void __ngtff_ss7__encode_callingPartyAddress( NGTFF_SM * sm, NGTFFSA * cpad);

void __ngtff_ss7__set_dataBuffer( NGTFF_SM * sm, uint8_t * buff, int len);

uint32_t __ngtff_ss7__get_localPointCode();

void __ngtff_ss7__get_protocol_data( uint8_t * msg, uint32_t * aspId, uint32_t * opc, uint32_t * dpc, uint8_t * ni, uint8_t * mp, uint8_t * sls);

int __ngtff_ss7__findAspByDPC( uint32_t dpc, uint32_t ** aspIds);

//return 1 on Active
int __ngtff_ss7__isAspActive( uint32_t aspId);

typedef void (*fp_ngtff_recvmsg)( uint8_t * msg);
int __ngtff_ss7__send_sccp( uint32_t aspId, uint32_t opc, uint32_t dpc, uint8_t ni, uint8_t mp, uint8_t sls, NGTFF_SM * sccpMsg, NGTFF_Error * ngErr);
int __ngtff_ss7__send_sccp2( uint8_t * msg, uint32_t opc, uint32_t dpc, uint8_t ni, uint8_t mp, uint8_t sls, NGTFF_SM * sccpMsg, NGTFF_Error * ngErr);
void __ngtff_ss7__onrecvmsg( fp_ngtff_recvmsg fc);

int __ngtff_ss7__sccp_encode_address( NGTFFSA * ad, uint8_t * uAddress, uint8_t * len);
int __ngtff_ss7__sccp_decode_address( NGTFFSA * ad, uint8_t * uAddress, uint8_t  len);

void __ngtff_ss7__print_buffer( char * buff, int len);

int __ngtff_ss7__encode_sccpmsg( NGTFF_SM * sm, char * buff, uint32_t * len);

int __ngtff_ss7__decode_sccp( uint8_t * msg, NGTFF_SM * sccpMsg);
void __ngtff_ss7__freemsg( uint8_t * msg);
void __ngtff_ss7__sccp_staticmsg_cleanup( NGTFF_SM * sccpMsg);
void __ngtff_makeTimeStamp( char * datestring);

int __ngtff_ss7__send_tm_inactive( uint32_t aspId);
int __ngtff_ss7__send_sm_aspdown( uint32_t aspId);

int __ngtff_ss7__ct( void *(*start_routine) (void *), void *arg, char * funcName);
int __ngtff_ss7__isAspConnected( uint32_t aspId);

#endif








