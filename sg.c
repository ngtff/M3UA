#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>


#include "ngtffm3ua.h"


int ngtff_send_test_msg( uint32_t opc, uint32_t dpc)
{
	NGTFF_SM sccpMsg;
	memset( &sccpMsg, 0, sizeof( NGTFF_SM));
	
	sccpMsg.MesageType = NGTFF_MESSAGE_TYPE__UNITDATA;
	sccpMsg.MesageClass = 0;
	sccpMsg.MesageHandling = 0;
	
	NGTFFSA calledAd;
	memset( &calledAd, 0, sizeof(NGTFFSA));
	
	calledAd.sai.pci 	= 0;
	calledAd.sai.ssni 	= 1;
	calledAd.sai.gti 	= 4;
	calledAd.sai.ri 	= 0;
	calledAd.sai.rnu 	= 0;
	
	calledAd.ssn = 6;
	
	calledAd.gt.tt = 0;
	calledAd.gt.np = 7;
	calledAd.gt.es = 2;	
	calledAd.gt.nai = 4;
	
	memcpy( calledAd.gt.d, "\x03\x63\x28", 3);
	calledAd.gt.dl = 3;
	
	// -------------------
	
	NGTFFSA callingAd;
	memset( &callingAd, 0, sizeof(NGTFFSA));
	
	callingAd.sai.pci 	= 0;
	callingAd.sai.ssni 	= 1;
	callingAd.sai.gti 	= 4;
	callingAd.sai.ri 	= 0;
	callingAd.sai.rnu 	= 0;
	
	callingAd.ssn = 7;
	
	callingAd.gt.tt = 0;
	callingAd.gt.np = 1;
	callingAd.gt.es = 2;	
	callingAd.gt.nai = 4;
	
	memcpy( callingAd.gt.d, "\x02\x60\x30\x11\x31", 5);
	callingAd.gt.dl = 5;
	
	// -------------------
	
	__ngtff_ss7__encode_calledPartyAddress( &sccpMsg, &calledAd);
	__ngtff_ss7__encode_callingPartyAddress( &sccpMsg, &callingAd);
	
	__ngtff_ss7__set_dataBuffer( &sccpMsg, "\x62\x52\x48\x04\xa6\x0a\x00\x00\x6b\x1e\x28\x1c\x06\x07\x00\x11" \
"\x86\x05\x01\x01\x01\xa0\x11\x60\x0f\x80\x02\x07\x80\xa1\x09\x06" \
"\x07\x04\x00\x00\x01\x00\x01\x03\x6c\x2a\xa1\x28\x02\x01\x78\x02" \
"\x01\x02\x30\x20\x04\x08\x79\x87\x08\x00\x00\x00\x00\xf1\x81\x06" \
"\x91\x02\x60\x30\x11\x31\x04\x06\x91\x02\x60\x30\x11\x31\x8a\x04" \
"\x00\x00\x00\x00", 84);
	
	NGTFF_Error ngError;
	
	uint32_t aspId = 1;
	int sentBytes = __ngtff_ss7__send_sccp( aspId, opc, dpc, 0, 5, 1, &sccpMsg, &ngError);
	//printf( "sentBytes=%d errno:%d error-msg:%s\n", sentBytes, ngError.ErrNo, ngError.Msg);
	__ngtff_ss7__sccp_staticmsg_cleanup( &sccpMsg);
	return sentBytes;
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
		printf( "syntax: %s ./ss7_config/ss7.txt\n\n", argv[0]);
		exit(0);
	}
	
	__ngtff_ss7__init();
	__ngtff_ss7__load_stackconfig( argv[1]);
	
	sleep(2);
	
	/*
	printf("sending message \n");
	
	int i = 0;
	int c = 0;
	int totCount = 0;
	int sentBytes = 0;
	
	char date[27];
	
	
	while(1)
	{
		for( i = 0; i < 1000; i++)
		{
			sentBytes += ngtff_send_test_msg( 200, 300);
			totCount++;
		}
		c++;
		
		__ngtff_makeTimeStamp( date);
		printf( "%s c=%d totCount=%d sentBytes=%d\n", date, c, totCount, sentBytes);
		sleep(1);
	}
	*/
	
	while(1) 
	{
		sleep(1);
	}
}




