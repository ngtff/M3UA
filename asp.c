#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>


#include "ngtffm3ua.h"




int asp_isvalid_sccpmsg( NGTFF_SM * sccpMsg)
{
	if( sccpMsg->calledPartyAddresslen == 0 || sccpMsg->callingPartyAddresslen == 0 || sccpMsg->sccpDatalen == 0) {
		return -1;
	}
	if( !sccpMsg->calledPartyAddress || !sccpMsg->callingPartyAddress || !sccpMsg->sccpData) {
		return -2;
	}
	return 1;
}

void asp_print_address( NGTFFSA * sccpAddress)
{
	return;
	
	printf( "Called Party Address Info : PCI=%u SSNI=%u GTI=%u RI=%u\n", sccpAddress->sai.pci, sccpAddress->sai.ssni, sccpAddress->sai.gti, sccpAddress->sai.ri);
	printf( "SPC=%u, SSN=%u\n", sccpAddress->spc, sccpAddress->ssn);
	printf( "GT OE=%u NAI=%u NP=%u ES=%u TT=%u dl=%u\n", sccpAddress->gt.oe, sccpAddress->gt.nai, sccpAddress->gt.np, sccpAddress->gt.es, sccpAddress->gt.tt, sccpAddress->gt.dl);
	
	__ngtff_ss7__print_buffer( sccpAddress->gt.d, sccpAddress->gt.dl);	
}


void asp_onrecvmsg( uint8_t * msg)
{
	//printf( "received sccp message:%p %s|%s|%d\n", msg, __FUNCTION__, __FILE__, __LINE__);
	
	uint32_t aspId;
	uint32_t opc;
	uint32_t dpc;
	uint8_t ni;
	uint8_t mp;
	uint8_t sls;
	
	__ngtff_ss7__get_protocol_data( msg, &aspId, &opc, &dpc, &ni, &mp, &sls);
	
	//printf( "aspId=%u opc=%u dpc=%u ni=%u mp=%u sls=%u   %s|%s|%d\n", aspId, opc, dpc, ni, mp, sls, __FILE__, __FUNCTION__, __LINE__);
	
	NGTFF_SM sccpMsg;
	memset( &sccpMsg, 0, sizeof( NGTFF_SM));
	
	int decode_sts = __ngtff_ss7__decode_sccp( msg, &sccpMsg);

	if( decode_sts != 1)
	{
		//decoding failed
		return;
	}
	
	if( asp_isvalid_sccpmsg( &sccpMsg) < 0)
	{
		return;
	}
	
	/*
	printf( "aspId=%u opc=%u dpc=%u ni=%u mp=%u sls=%u decode_sts=%u MesageType=%u %s|%s|%d\n", 
		aspId, opc, dpc, ni, mp, sls, decode_sts, sccpMsg.MesageType, __FUNCTION__, __FILE__, __LINE__);
	*/
	
	NGTFFSA calledPartyAddress;
	memset( &calledPartyAddress, 0, sizeof( NGTFFSA));
	
	NGTFFSA callingPartyAddress;
	memset( &callingPartyAddress, 0, sizeof( NGTFFSA));	
	
	decode_sts = __ngtff_ss7__sccp_decode_address( &calledPartyAddress, sccpMsg.calledPartyAddress , sccpMsg.calledPartyAddresslen);
	
	if( decode_sts == 1)
		asp_print_address( &calledPartyAddress);
	
	decode_sts = __ngtff_ss7__sccp_decode_address( &callingPartyAddress, sccpMsg.callingPartyAddress , sccpMsg.callingPartyAddresslen);
	
	if( decode_sts == 1)
		asp_print_address( &calledPartyAddress);
	
}

void ngtff_send_test_msg( uint32_t opc, uint32_t dpc)
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
	
	/*
	uint8_t uadd[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t uaddlen = 0;
	__ngtff_ss7__encode_address( &calledAd, uadd, &uaddlen);
	__ngtff_ss7__print_buffer( uadd, uaddlen);
	*/
	
	// -------------------
	
	__ngtff_ss7__encode_calledPartyAddress( &sccpMsg, &calledAd);
	__ngtff_ss7__encode_callingPartyAddress( &sccpMsg, &callingAd);
	
	__ngtff_ss7__set_dataBuffer( &sccpMsg, "\x62\x52\x48\x04\xa6\x0a\x00\x00\x6b\x1e\x28\x1c\x06\x07\x00\x11" \
"\x86\x05\x01\x01\x01\xa0\x11\x60\x0f\x80\x02\x07\x80\xa1\x09\x06" \
"\x07\x04\x00\x00\x01\x00\x01\x03\x6c\x2a\xa1\x28\x02\x01\x78\x02" \
"\x01\x02\x30\x20\x04\x08\x79\x87\x08\x00\x00\x00\x00\xf1\x81\x06" \
"\x91\x02\x60\x30\x11\x31\x04\x06\x91\x02\x60\x30\x11\x31\x8a\x04" \
"\x00\x00\x00\x00", 84);
	
	
	/*
	uint8_t sccpmsgbuffer[500];
	memset( &sccpmsgbuffer, 0, sizeof( sccpmsgbuffer));
	uint32_t sccpMsgLen = 0;
	
	__ngtff_ss7__encode_sccpmsg( &sccpMsg, sccpmsgbuffer, &sccpMsgLen);
	__ngtff_ss7__print_buffer( sccpmsgbuffer, sccpMsgLen);
	*/
	
	NGTFF_Error ngError;
	
	int sentBytes = __ngtff_ss7__send_sccp( 1, 410, 400, 0, 5, 1, &sccpMsg, &ngError);
	printf( "sentBytes=%d\n", sentBytes);
}


uint32_t ngtff_capture_int_input( char * prompt, uint32_t min, uint32_t max, uint32_t localOpc)
{
	char option[5];
	uint32_t input = 0;

	while(1)
	{
		printf("1 Please enter (min:%d max:%d) %s:", min, max, prompt);
		
		scanf( "%s", option);
		input = atoi( option);
		
		if( input >= min && input <= max && localOpc != input)
		{
			return input;
		}
		else
		{
			printf("please enter in valid range min:%d max:%d localOpc:%d, entered value:%d\n", min, max, localOpc, input);
		}
	}
}

void ngtff_capture_inputs_and_send_sccp_message()
{
	uint32_t opc = __ngtff_ss7__get_localPointCode();
	uint32_t dpc = ngtff_capture_int_input("DPC", 1, 999, opc);
	
	ngtff_send_test_msg( opc, dpc);
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
	__ngtff_ss7__onrecvmsg( asp_onrecvmsg);
	
	
	
	sleep(2);
	
	//ngtff_send_test_msg( 300, 320);
	
	/*
	uint16_t pointCode = 902;
	uint8_t dx[2] = {0,0}; 
	
	dx[0] = (pointCode) & 0xFF;
	dx[1] = (pointCode >> 8) & 0xFF;
	
	printf( "dx[%02X] dx[%02X]\n", dx[0] & 0xFF, dx[1] & 0xFF);
	*/
	
	/*
	NGTFFSA cpad;
	memset( &cpad, 0, sizeof( NGTFFSA));
	
	cpad.sai.pci = 1;
	cpad.sai.ri = 1;
	cpad.sai.ssni = 1;
	cpad.spc = 902;
	
	uint8_t uadd[4] = {0,0,0,0};
	int uaddlen = 0;
	
	__ngtff_ss7__encode_address( &cpad, uadd, &uaddlen);
	__ngtff_ss7__print_buffer( uadd, uaddlen);
	*/
	
	char option[5];
	uint32_t selected = 0;
	
	while(1)
	{
		printf("1 Send SCCP Message:\n");
		printf("2 Exit:\n");
		
		printf("please choose option:");
		scanf( "%s", option);
		selected = atoi( option);
		
		if( selected >= 1 && selected <= 2)
		{
			switch( selected)
			{
				case 1:
					ngtff_capture_inputs_and_send_sccp_message();
					break;
				case 2:
					exit(0);
					break;	// make compiler happy
				default:
					break;
			}
		}
		else
		{
			printf("invalid option\n");
		}
	}
	

}




