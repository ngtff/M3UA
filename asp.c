#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>


#include "ngtffm3ua.h"

void ngtff_recv_sccpmsg( uint32_t aspId, uint32_t opc, uint32_t dpc, uint8_t ni, uint8_t mp, uint8_t sls, SI_NGTFF_SCCPMsg * sccpMsg)
{
	
}

/*
	cat /proc/net/sctp/eps
	lsof -R | grep 1895802
*/
int main( int argc, char **argv)
{
	if( argc < 2)
	{
		printf( "missing configfile\n");
		printf( "syntax: %s ./ss7_config/ss7client.txt\n\n", argv[0]);
		exit(0);
	}
	
	__ngtff_ss7__init();
	__ngtff_ss7__load_stackconfig( argv[1]);
	__ngtff_ss7__on_recv_sccp_msg( ngtff_recv_sccpmsg);
	
	while(1) 
	{
		sleep(1);
	}
}




