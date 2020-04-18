#ifndef _DEFINITON_
#define _DEFINITION_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <logger.h>

#include <interop.h>
#include <SocketFramework.h>
#include <smpplib.h>
#include <mysql.h>
#include "threadSafeLog.h"

//#include <snmplib.h>

#ifdef COMPILER_GCC
#include <stdarg.h> 
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>	
#include <dirent.h>		// Needed for getting linux process information by parsing /proc 
#endif

// This holds the process name
extern char *__progname;



char	SERVER_NAME[200];
char	DB_USER[20];
char	DB_USERPASS[20];
char	DB_NAME[100];

char GetURL1[200];
char GetURL2[200];
char error_time[100];
char error_msg[2000];
char error_reason[1000];
char error_in_func[100];

int  WEB_HTTP_PORT;
int  PORT_FOR_CDE;
char IP_FOR_CDE[20];
char CDE_URL[500];
char SERVICECONFIG_URL[200];
char SHORTCODE_URL[200];
char OPERATORINFO_URL[200];

bool testbinarysms = false;
int  Invalid = false;
int  BIANRYSMS = 0;

char LOG_DIRNAME[1000];

int  MAX_NUMBER_OF_ROW_FETCH;
int APPEND_SHORTCODE_PREFIX;
int SHORTCODE_PREFIX_VALUE;

int  SMSGW_THREAD_STARTED = 0;
int  SMSLOG_ENABLED = 0;

char CDE_FAILED_SMS[161];
int  HEART_BEAT_DELAY = 20;
int  RETRYCOUNT = 5;
int  RETRY_DELAY = 10; // second
int  PING_BALL_DELAY = 30; // minute

int  SNMP_ENABLED;
char SNMP_MANAGER_HOST_IP[15000];
int  SNMP_MANAGER_PORT_NO;
char SNMP_LOCAL_IP[15000];
int  SNMP_LOCAL_PORT;
char SNMP_AGENT_IP[15000];
int	 SNMP_AGENT_IP_SPACED[4];
bool isDBConnectionFailed;
bool isSMSCConnectionFailed;
int  LOG_LEVEL;
int  LOG_DESTINATION;
char LOG_HOST[100];
char UPDATE_STATUS[20];
int  LOG_PORT;
char LOG_COMPONENT[100];
char SNMP_INSTANCE_NAME[100];
int  DELIVERED_TIME_DELAY;
int  DELIVERY_RECEIVED_ENNABLE;

int  CONTENT_THREAD_COUNT;
int  OUTBOX_THREAD_COUNT;
char SUBMISSION_RETRY_GAP[25];
int  SUBMISSION_DELAY;
char RETRY_DELAY_UOM[10];
char SUBMISSION_DELAY_UOM[10];

char SMSC_IP[50];
int	 SMSC_PORT, TON, NPI, SRCTON, SRCNPI, DESTTON, DESTNPI;
char SMSC_USERNAME[20], SMSC_PASSWORD[20];
int	 CONMODE;
char INTERIM_STATUS[15];

/*********SMSClientID**************/
char SMS_CLIENT_ID[50];
/******TPS COUNTER*****************/
int  TPS;
byte TPS_RECORDS[24][60][60];
int  TPS_LOG_ENABLE;
volatile int TOTAL_SERVICE_COUNT = 0;
volatile int TOTAL_SHORTCODE_COUNT = 0;
volatile int QUED_SHORTCODE_COUNT = 0;
int TOTAL_OPERATOR_COUNT = 1;
SOCKET sContent;
int HTTP_TPS = 250; // Need to confirm from operator.
int SHORTCODE_TPS = 100; // Need to confirm from operator.

#define PDU_STRING_LENGTH 1500
#define MAX_SMPP_ACCOUNT_COUNT 30
#define MAX_SMSGW_APPLICATION_COUNT 30
#define MAX_SMS_COUNT 100
#define MAX_TPS_COUNT 3000
#define MAX_SERVICE_COUNT 3000
#define MAX_SHORTCODE_COUNT 1000
#define MAX_OPERATOR_COUNT 20
#define URL_SIZE 5000


#define MAXQUERYLENGTH 10000
#define MAXCOULMNLENGTH 30
#define CURL_BATCH_SIZE 50

struct httpResponseBuf {
	char buf[1000];
	size_t size;
};
struct httpResponseBuf chunkResponse[CURL_BATCH_SIZE*MAX_OPERATOR_COUNT];




/******For log writting*******/
char LOG_STRING[5000], LOG_DIRECTORY[100];
int LOG_FILTER, LOG_FILTER_OPTION;

bool BINDCOMPLETED = false;


int SEQUENCENO = 2;

int  CDE_CURL_BATCH_SIZE = 50;
int MAX_SMS_IN_QUE = 1000;
volatile int COUNT_SMS_IN_APP_QUE = 0;

/*******For send to SMSC*******/
typedef struct{
	char ip[20];
	int port;
	SOCKET socketToSMSC;
} smsc;

smsc SMSCINFO;

bool MYSQLINITNOTPOSSIBLE = false;
bool FIRSTENQUIRELINKRECEIVED = false;

int	PTHREAD_STACK_SIZE_NETWORK_KB = 1024;
int	PTHREAD_STACK_SIZE_KB = 1024;
int BIN_SEGMENT_SIZE=134;

int ADD_UDH;
char UDH_STREAM[100];

/*****Trap variables******/

#define SNMP_TRAP_ID 4001
#define SNMP_OID_PART {1, 10, 35}

volatile bool bReloadingShortcode = false;
volatile bool bThreadStarted = false;


/*******FOR NUMBER SPECIFIC LOG*********/
int NUMBER_SPECIFIC_LOG_ENABLE;
char NUMBER1[20];
char NUMBER2[20];

/********CommandProcess***************/

#define MAXCOMMANDLENGTH 1000
int COMMAND_PROCESS_PORT;

char **NOREPLY_STRINGS;
int NOREPLY_STRING_NUMBER = 0;
int USE_DEFAULT_STATUS = 1;

// Channel Configuration Flag Variable
int HTTP = 0, SMPP = 0;

// MakeQueOrFail thread configuration flag
int IS_REQUE_ENABLE = 0;

volatile bool isPingSent = true;

struct cdebuffer {
  char buf[200000];
  size_t size;
};

// Submit_SM_RESP sequence number queue
 void insert_q(int no);
 int delete_q();
 void display();
 void *SubmitSMRespHandler(void *p);

struct link {
	int info;
	struct link *next;
}*front, *rear;

static size_t  WriteBufferCallback(void *contents, size_t size, size_t nmemb, void *userp);


typedef struct serviceConfig{

	int id;
	char OperatorID[50];
	char Shortcode[50];
	char ServiceID[500];
	char SubscriptionGroupID[500];
	int ChargeParty;
	int CallCGWURL;
	int TPS;
	char cgwUserID[50];
	char cgwPassword[50];
	int Priority;
	bool exists;
} serviceConfig;

serviceConfig serviceConfigStruct[MAX_SERVICE_COUNT];

typedef struct smsinboxShortcodes {
	char shortcode[50];	
} smsinboxShortcodes;
smsinboxShortcodes queuedSmsinboxShortcodes[MAX_SHORTCODE_COUNT];

typedef struct smsoutbox{

	char srcMN[50];
	char dstMN[100];
	char msg[500];
	char msgType[20];
	char serviceID[500];	
	int isChargingEnabled;	
	int channel; //0 for HTTP; 1 for SMPP
	char refID[50];
	char destAccount[20];
} smsoutbox;

typedef struct smsinbox
{
	char srcMN[50];
	char dstMN[100];
	char msg[500];
	int channel; //0 for HTTP; 1 for SMPP
	int msgID;
	int retryCount;
	char refID[50];
	char srcAccount[20];
} smsinbox;

struct index_msgId_map {	
	int index;		
	smsinbox sms;
};

typedef struct cde_failed
{
	int msgID;
} cde_failed;

typedef struct queue2DReference{
	int serviceConfigID;
	bool exists;
} queue2DReference;
queue2DReference queue2DReferenceStruct[MAX_SERVICE_COUNT];

typedef struct queue2DPriority{
	int serviceConfigID;
	int priority;
	bool exists;
} queue2DPriority;
queue2DPriority queue2DPriorityStruct[MAX_SERVICE_COUNT];

typedef struct shortcode_wise2d_que{
	char shortcode[20];
	int tps;
	int priority;
	bool exists;
} shortcode_wise2d_que;
shortcode_wise2d_que shortcode_wise2d_queStruct[MAX_SHORTCODE_COUNT];

typedef struct inbox_node{
	struct smsinbox sms;
	struct inbox_node *next;
} inbox_node, *pInbox_node;

typedef struct cde_failed_node{
	struct cde_failed sms;
	struct cde_failed_node *next;
} cde_failed_node, *pCde_failed_node;

typedef struct inbox_queue{
	pInbox_node Head;
	pInbox_node Tail;
} inbox_queue, *pInbox_queue;

typedef struct outbox_node{
	struct smsoutbox sms;
	struct outbox_node *next;
} outbox_node, *pOutbox_node;

typedef struct outbox_queue{
	pOutbox_node Head;
	pOutbox_node Tail;
} outbox_queue, *pOutbox_queue;


typedef struct node{
	struct smsoutbox sms;
	struct node *next;
} node, *pnode;

// Structure of Operator table for HTTP channel
typedef struct operatorConfig{	
	char MT_URL[5000];
	char operatorName[20];
} operatorConfig; 
operatorConfig operatorConfigStruct[MAX_OPERATOR_COUNT];
int operator_id[MAX_OPERATOR_COUNT];

pthread_mutex_t  queueLock_flag = PTHREAD_MUTEX_INITIALIZER;


// Queue variables
const int TOTAL_SERVICE = 100;
const int TOTAL_SHORTCODE = 100;

pInbox_queue shortcodeQueueArray[TOTAL_SHORTCODE], smsInboxQueue, smsInboxResponseQueue;
pOutbox_queue smsOutboxQueue;

// flag is on/off for emptying application queues 
volatile bool bReloadComplete = true; 

volatile bool OutboxSingleMemoryQuePingFlag = false, CDEFailedMemoryQuePingFlag = false, InboxResponseMemoryQuePingFlag = false;
volatile bool bSmsReceivingFromQue = false, bSmsBeingSent =false, bSmsBeingFailed = false, bSmsBeingQueInRetryFailed =false;
volatile bool bSmsDequeueing = false;


//pInbox_node pnew,temp;

#endif //_DEFINITON_



