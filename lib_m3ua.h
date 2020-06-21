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

#ifndef SI_LIB_M3UA_H
#define SI_LIB_M3UA_H

#include "sirik_core.h"

typedef struct __si_m3ua_error
{
	int ErrNo;
	u_char Msg[249];
} SI_M3UAError;

struct tok
{
	uint32_t id;
	u_char * val;
};

/* message classes */
#define SI_M3UA_MSGC_MGMT 								0
#define SI_M3UA_MSGC_TRANSFER 							1
#define SI_M3UA_MSGC_SSNM 								2
#define SI_M3UA_MSGC_ASPSM 								3
#define SI_M3UA_MSGC_ASPTM 								4
/* reserved values */
#define SI_M3UA_MSGC_RKM 								9


static const struct tok MessageClasses[] = {
	{ SI_M3UA_MSGC_MGMT,     "Management"            },
	{ SI_M3UA_MSGC_TRANSFER, "Transfer"              },
	{ SI_M3UA_MSGC_SSNM,     "SS7"                   },
	{ SI_M3UA_MSGC_ASPSM,    "ASPSM"                 },
	{ SI_M3UA_MSGC_ASPTM,    "ASPTM"                 },
	{ SI_M3UA_MSGC_RKM,      "Routing Key Management"},
	{ 0, NULL }
};


/* management messages */
#define SI_M3UA_MGMT_ERROR 								0
#define SI_M3UA_MGMT_NOTIFY 							1


static const struct tok MgmtMessages[] = {
  { SI_M3UA_MGMT_ERROR, "Error" },
  { SI_M3UA_MGMT_NOTIFY, "Notify" },
  { 0, NULL }
};



/* transfer messages */
#define SI_M3UA_TRANSFER_DATA 1

static const struct tok TransferMessages[] = {
  { SI_M3UA_TRANSFER_DATA, "Data" },
  { 0, NULL }
};


/* SS7 Signaling Network Management messages */
#define SI_M3UA_SSNM_DUNA 								1
#define SI_M3UA_SSNM_DAVA 								2
#define SI_M3UA_SSNM_DAUD 								3
#define SI_M3UA_SSNM_SCON 								4
#define SI_M3UA_SSNM_DUPU 								5
#define SI_M3UA_SSNM_DRST 								6

static const struct tok SS7Messages[] = {
  { SI_M3UA_SSNM_DUNA, "Destination Unavailable" },
  { SI_M3UA_SSNM_DAVA, "Destination Available" },
  { SI_M3UA_SSNM_DAUD, "Destination State Audit" },
  { SI_M3UA_SSNM_SCON, "Signalling Congestion" },
  { SI_M3UA_SSNM_DUPU, "Destination User Part Unavailable" },
  { SI_M3UA_SSNM_DRST, "Destination Restricted" },
  { 0, NULL }
};


/* ASP State Maintenance messages */
#define SI_M3UA_ASP_UP 									1
#define SI_M3UA_ASP_DN 									2
#define SI_M3UA_ASP_BEAT 								3
#define SI_M3UA_ASP_UP_ACK 								4
#define SI_M3UA_ASP_DN_ACK 								5
#define SI_M3UA_ASP_BEAT_ACK 							6

static const struct tok ASPStateMessages[] = {
  { SI_M3UA_ASP_UP, "Up" },
  { SI_M3UA_ASP_DN, "Down" },
  { SI_M3UA_ASP_BEAT, "Heartbeat" },
  { SI_M3UA_ASP_UP_ACK, "Up Acknowledgement" },
  { SI_M3UA_ASP_DN_ACK, "Down Acknowledgement" },
  { SI_M3UA_ASP_BEAT_ACK, "Heartbeat Acknowledgement" },
  { 0, NULL }
};


/* ASP Traffic Maintenance messages */
#define SI_M3UA_ASP_AC 									1
#define SI_M3UA_ASP_IA 									2
#define SI_M3UA_ASP_AC_ACK 								3
#define SI_M3UA_ASP_IA_ACK 								4

static const struct tok ASPTrafficMessages[] = {
  { SI_M3UA_ASP_AC, "Active" },
  { SI_M3UA_ASP_IA, "Inactive" },
  { SI_M3UA_ASP_AC_ACK, "Active Acknowledgement" },
  { SI_M3UA_ASP_IA_ACK, "Inactive Acknowledgement" },
  { 0, NULL }
};


/* Routing Key Management messages */
#define SI_M3UA_RKM_REQ 								1
#define SI_M3UA_RKM_RSP 								2
#define SI_M3UA_RKM_DEREQ 								3
#define SI_M3UA_RKM_DERSP 								4

static const struct tok RoutingKeyMgmtMessages[] = {
  { SI_M3UA_RKM_REQ, "Registration Request" },
  { SI_M3UA_RKM_RSP, "Registration Response" },
  { SI_M3UA_RKM_DEREQ, "Deregistration Request" },
  { SI_M3UA_RKM_DERSP, "Deregistration Response" },
  { 0, NULL }
};


/* M3UA Parameters */
#define SI_M3UA_PARAM_INFO 								0x0004			// UTF-8  String variable length
#define SI_M3UA_PARAM_ROUTING_CTX 						0x0006			// n x 32 bits (unsigned integer)
#define SI_M3UA_PARAM_DIAGNOSTIC 						0x0007
#define SI_M3UA_PARAM_HB_DATA 							0x0009
#define SI_M3UA_PARAM_TRAFFIC_MODE_TYPE 				0x000b
#define SI_M3UA_PARAM_ERROR_CODE 						0x000c
#define SI_M3UA_PARAM_STATUS 							0x000d
#define SI_M3UA_PARAM_ASP_ID 							0x0011			//	32-bit unsigned integer
#define SI_M3UA_PARAM_AFFECTED_POINT_CODE 				0x0012			//  n x 32 bits
#define SI_M3UA_PARAM_CORR_ID 							0x0013

#define SI_M3UA_PARAM_NETWORK_APPEARANCE 				0x0200			//  32-bit unsigned integer
#define SI_M3UA_PARAM_USER 								0x0204
#define SI_M3UA_PARAM_CONGESTION_INDICATION 			0x0205			// 32 bits
#define SI_M3UA_PARAM_CONCERNED_DST 					0x0206			// 32 bits
#define SI_M3UA_PARAM_ROUTING_KEY 						0x0207
#define SI_M3UA_PARAM_REG_RESULT 						0x0208
#define SI_M3UA_PARAM_DEREG_RESULT 						0x0209
#define SI_M3UA_PARAM_LOCAL_ROUTING_KEY_ID 				0x020a
#define SI_M3UA_PARAM_DST_POINT_CODE 					0x020b
#define SI_M3UA_PARAM_SI 								0x020c
#define SI_M3UA_PARAM_ORIGIN_POINT_CODE_LIST 			0x020e
#define SI_M3UA_PARAM_PROTO_DATA 						0x0210
#define SI_M3UA_PARAM_REG_STATUS 						0x0212
#define SI_M3UA_PARAM_DEREG_STATUS 						0x0213


// Congestion Level field: 8 bits (unsigned integer)
// Mask: 8 bits (unsigned integer)
// User/Cause: 32 bits
// Unavailability Cause field: 16 bits (unsigned integer)
// MTP3-User Identity field: 16 bits (unsigned integer)
// ASP Identifier: 32-bit unsigned integer
// Routing Key: variable length
// Local-RK-Identifier: 32-bit unsigned integer
// Traffic Mode Type: 32-bit (unsigned integer)
// Service Indicators (SI): n X 8-bit integers
// Local-RK-Identifier: 32-bit integer

static const struct tok ParamName[] = {
  { SI_M3UA_PARAM_INFO, "INFO String" },
  { SI_M3UA_PARAM_ROUTING_CTX, "Routing Context" },
  { SI_M3UA_PARAM_DIAGNOSTIC, "Diagnostic Info" },
  { SI_M3UA_PARAM_HB_DATA, "Heartbeat Data" },
  { SI_M3UA_PARAM_TRAFFIC_MODE_TYPE, "Traffic Mode Type" },
  { SI_M3UA_PARAM_ERROR_CODE, "Error Code" },
  { SI_M3UA_PARAM_STATUS, "Status" },
  { SI_M3UA_PARAM_ASP_ID, "ASP Identifier" },
  { SI_M3UA_PARAM_AFFECTED_POINT_CODE, "Affected Point Code" },
  { SI_M3UA_PARAM_CORR_ID, "Correlation ID" },
  { SI_M3UA_PARAM_NETWORK_APPEARANCE, "Network Appearance" },
  { SI_M3UA_PARAM_USER, "User/Cause" },
  { SI_M3UA_PARAM_CONGESTION_INDICATION, "Congestion Indications" },
  { SI_M3UA_PARAM_CONCERNED_DST, "Concerned Destination" },
  { SI_M3UA_PARAM_ROUTING_KEY, "Routing Key" },
  { SI_M3UA_PARAM_REG_RESULT, "Registration Result" },
  { SI_M3UA_PARAM_DEREG_RESULT, "Deregistration Result" },
  { SI_M3UA_PARAM_LOCAL_ROUTING_KEY_ID, "Local Routing Key Identifier" },
  { SI_M3UA_PARAM_DST_POINT_CODE, "Destination Point Code" },
  { SI_M3UA_PARAM_SI, "Service Indicators" },
  { SI_M3UA_PARAM_ORIGIN_POINT_CODE_LIST, "Originating Point Code List" },
  { SI_M3UA_PARAM_PROTO_DATA, "Protocol Data" },
  { SI_M3UA_PARAM_REG_STATUS, "Registration Status" },
  { SI_M3UA_PARAM_DEREG_STATUS, "Deregistration Status" },
  { 0, NULL }
};


// ---------------------------------------------------------------------------------------------
// M3UA Message

// Parameter Value Type
#define SI_M3UA_PVT_08BIT_UINT				1
#define SI_M3UA_PVT_16BIT_UINT				2
#define SI_M3UA_PVT_32BIT_UINT				3
//#define SI_M3UA_PVT_64BIT_UINT				4
#define SI_M3UA_PVT_08BIT_SINT				5
#define SI_M3UA_PVT_16BIT_SINT				6
#define SI_M3UA_PVT_32BIT_SINT				7
//#define SI_M3UA_PVT_64BIT_SINT				8
#define SI_M3UA_PVT_STRING					9


typedef struct __si_m3ua_protocol_data
{
	uint32_t OPC;
	uint32_t DPC;
	uint8_t  SI;		// Service Information Octet
	uint8_t  NI;		// National Network
	uint8_t  MP;		// Message Priority
	uint8_t SLS;		// Signalling Link Selection	
	uint8_t * data;		// User Protocol Data: variable-length octet string
	uint32_t datalen;
} SI_M3UAProtocolData;

typedef struct __si_m3ua_parameter
{
	struct __si_m3ua_parameter * Next;
	
	uint16_t PVT;	
	uint16_t Tag;
	uint16_t Length;	//length of total message
	
	uint32_t u8Val;
	uint32_t u16Val;
	uint32_t u32Val;

	int32_t i8Val;
	int32_t i16Val;
	int32_t i32Val;
	
	uint8_t * data;
	uint32_t datalen;
	
	uint32_t isFree;
	
} SI_M3UAParameter;

typedef struct __si_m3ua_mgs
{
	struct __si_m3ua_mgs * Next;
	
	uint8_t  Version;
	uint8_t  Reserved;
	uint8_t  MessageClass;
	uint8_t  MessageType;
	uint32_t MessageLength;	
	
	SI_M3UAParameter * pHead;
	SI_M3UAParameter * pCurrent;
	
	uint32_t isFree;
	uint8_t * siSocket;
	uint16_t streamId;
	uint16_t ppid;
	
	uint32_t AspId;
	uint32_t OPC;
	uint32_t DPC;
	uint8_t  SI;		// Service Information Octet
	uint8_t  NI;		// National Network
	uint8_t  MP;		// Message Priority
	uint8_t SLS;		// Signalling Link Selection
	
	uint8_t * payloadPtr;
	uint32_t payloadlen;
} SI_M3UA_Msg;

// ---------------------------------------------------------------------------------------------

void __si_m3ua__init_message( SI_M3UA_Msg * msg, uint8_t class, uint8_t type);
void __si_m3ua__encode_message( SI_M3UA_Msg * msg, u_char * buff, uint32_t * len);
void __si_m3ua__free_message( SI_M3UA_Msg * msg, int freeMessage, int freeParameters);
SI_M3UA_Msg * __si_m3ua__decode_message( char * buff, int len);
int __si_m3ua__IsValidMessageType( uint8_t MessageClass, uint8_t MessageType);
void __si_m3ua__print_message( SI_M3UA_Msg * msg, int printParams);
void __si_m3ua__log_message( SI_M3UA_Msg * msg, char * file, const char * func, int line);


void __si_m3ua__add_string_parameter( SI_M3UA_Msg * msg, uint16_t type, uint8_t * value, uint32_t len);

void __si_m3ua__add_u8int_parameter( SI_M3UA_Msg * msg, uint16_t type, uint8_t val);
void __si_m3ua__add_u16int_parameter( SI_M3UA_Msg * msg, uint16_t type, uint16_t val);
void __si_m3ua__add_u32int_parameter( SI_M3UA_Msg * msg, uint16_t type, uint32_t val);

void __si_m3ua__add_s8int_parameter( SI_M3UA_Msg * msg, uint16_t type, int8_t val);
void __si_m3ua__add_s16int_parameter( SI_M3UA_Msg * msg, uint16_t type, int16_t val);
void __si_m3ua__add_s32int_parameter( SI_M3UA_Msg * msg, uint16_t type, int32_t val);

void __si_m3ua__add_network_appearance( SI_M3UA_Msg * msg, uint32_t val);
void __si_m3ua__add_error_code( SI_M3UA_Msg * msg, uint32_t uErrorCode);
void __si_m3ua__add_routing_context( SI_M3UA_Msg * msg, uint32_t val);
void __si_m3ua__add_protocol_data( SI_M3UA_Msg * msg, SI_M3UAProtocolData * protocolData);
void __si_m3ua__add_correlation_id( SI_M3UA_Msg * msg, uint32_t val);
void __si_m3ua__add_affected_point_code( SI_M3UA_Msg * msg, uint32_t val);
void __si_m3ua__add_info_string( SI_M3UA_Msg * msg, uint8_t * value, uint32_t len);
void __si_m3ua__add_concerned_destination( SI_M3UA_Msg * msg, uint32_t val);
void __si_m3ua__add_congestion_indications( SI_M3UA_Msg * msg, uint32_t val);
void __si_m3ua__add_user_cause( SI_M3UA_Msg * msg, uint16_t cause, uint16_t userId);
void __si_m3ua__add_asp_identifier( SI_M3UA_Msg * msg, uint32_t val);
void __si_m3ua__add_beat_data( SI_M3UA_Msg * msg, uint8_t * data, uint32_t len);
void __si_m3ua__add_traffic_mode_type( SI_M3UA_Msg * msg, uint32_t val);
void __si_m3ua__add_status( SI_M3UA_Msg * msg, uint16_t statsType, uint16_t statusInformation);
void __si_m3ua__add_diagnostic_information( SI_M3UA_Msg * msg, uint8_t * data, uint32_t len);

SI_M3UAParameter * __si_m3ua_get_parameter_byTagId( SI_M3UA_Msg * msg, uint16_t tagId, int index);

SI_M3UAParameter * __si_m3ua_allocate_parameter();
void __si_m3ua_release_parameter( SI_M3UAParameter * parameter);

void __si_m3ua_release_msg( SI_M3UA_Msg * msg);
SI_M3UA_Msg * __si_m3ua_allocate_msg();


uint32_t __si_m3ua__version();
void __si_m3ua__init();
void __si_m3ua__handle_datamsg( SI_M3UA_Msg * msg);

void __si_m3ua__add_parameter( SI_M3UA_Msg * msg, SI_M3UAParameter * parameter, uint8_t pvt, uint16_t tag);
int __si_m3ua__layer_sendmsg( uint32_t aspId, SI_M3UA_Msg * msg, SI_M3UAError * siErr);

typedef void (*f_si_on_sccp_msg)( SI_M3UA_Msg * msg);
void __si_m3ua__set_msghandler( f_si_on_sccp_msg handler);

#endif






