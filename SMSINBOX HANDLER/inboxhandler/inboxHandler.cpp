#include "functions.h"

int main(void) 
{

	curl_global_init(CURL_GLOBAL_DEFAULT);
	// Check another instance is already running
	if( true == isAnotherInstanceRunning())
	{
		printf("Another Instance is Running\n");
		return 0;
	}	
	
	// Thread and logging variables
	int TOTAL_THREAD = 30, threadCount = 0;
	pthread_attr_t attr[TOTAL_THREAD];
	pthread_t threadId[TOTAL_THREAD];

	// Initialize 2D array
	for(int i = 0; i < MAX_SHORTCODE_COUNT; i++) shortcode_wise2d_queStruct[i].exists = false;
	// Initializing reference and priority arrays
	for (int i = 0; i<MAX_SERVICE_COUNT; i++)	serviceConfigStruct[i].exists = false;	

	// Variables for time calculation
	time_t counter;
	struct tm timer;
	int prev_hour, current_hour,i;
	time(&counter);
	timer = *localtime(&counter);
	prev_hour = timer.tm_hour;
	current_hour = timer.tm_hour;

	// Initialize attribute structure for each thread
	while(threadCount < TOTAL_THREAD) initThreadAttr(&attr[threadCount++]);
	threadCount = 0;
	
	// Load configuration
	LoadConfig("main");

	if(1 == SMSLOG_ENABLED)
	{
		// initialize log queue
		//initializeIPCQForLog(); 

		// Log configuration setting
		setLogDestination(LOG_DESTINATION);	    // Destination of the Log is 10
		setLogLevel(LOG_LEVEL);					// The level of log to define the sms criteria. 8-ERROR, 32-DEBUG_MAJOR
		setLogServerAddress(LOG_HOST);          
		setLogPort(LOG_PORT);
		setComponentName(LOG_COMPONENT);

		// Start Thread safe log activity 
		//pthread_create(&threadId[threadCount], &attr[threadCount], writeLogActivity, NULL);
		//threadCount++;
	}
	

	if(NUMBER_SPECIFIC_LOG_ENABLE == 1 && strlen(NUMBER1) > 0) {
		if(strlen(NUMBER2) > 0)	setFilter(ON, NUMBER1, NUMBER2);
		else setFilter(ON, NUMBER1);
	}

	logActivity(__FUNCTION__, "SMSGW_Vs", E_ERROR, "------Inbox Handler Release 4.1.2.20180930--------");
		
	//int ret = loadShortcode();	
	int ret = loadShortcodeConfig();	
	if (ret != SUCCESS) {
		logActivity(__FUNCTION__, "shortcode", E_ERROR, " Error loading shortcode PHP file");
		return 0;
	}

	logActivity(__FUNCTION__, "shortcode", E_DEBUG_MAJOR, "TOTAL_SHORTCODE_COUNT :%d", TOTAL_SHORTCODE_COUNT);
	
	// Read service configuration from a url
	ret = loadServiceConfig();
	if (ret != SUCCESS) 
	{
		logActivity(__FUNCTION__, "servconf", E_ERROR, " Error loading service configuration PHP file");		
		return 0;
	}
		
	ret = loadOperatorInfo();
	if (ret != SUCCESS) 
	{
		logActivity(__FUNCTION__, "operatorInfo", E_ERROR, " Error loading OperatorInfo PHP file");
		return 0;
	}
	
	//MAX_SMS_IN_QUE = MAX_NUMBER_OF_ROW_FETCH*10;
	
	logActivity(__FUNCTION__, "ipcQue", E_DEBUG_MAJOR, "MAX_SMS_IN_QUE %d", MAX_SMS_IN_QUE);
		
	// IPC Initialization for Inbox_handler	
	initializeMemoryQue();
	

	// Queue creation
	for ( i = 0; i < TOTAL_SHORTCODE_COUNT; i++) shortcodeQueueArray[i] = createInboxQueue();
	
	bThreadStarted = false;
	// Reinitialize SMS status update handler thread
	pthread_create(&threadId[threadCount], &attr[threadCount], ReinitializeSMS, NULL);
	threadCount++;
		
	// Thread to pick SMS from database with state = QUE
	for ( i = 0; i < 1; i++)
	{
		while (false == bThreadStarted) Sleep(MIN_SLEEP);	
		bThreadStarted = false;
		pthread_create(&threadId[threadCount], &attr[threadCount], PickQUEDSMSFromInbox, NULL);
		threadCount++;
	}
	
	
	//for ( i = 0; i < CDE_THREAD_COUNT; i++)
	//{
	//	while (false == bThreadStarted) Sleep(MIN_SLEEP);	
	//	bThreadStarted = false;
	//	//logActivity(__FUNCTION__,"cpp", E_ERROR, "i:%d", i);
	//	pthread_create(&threadId[threadCount], &attr[threadCount], CallCDEURL, (void *)&i);
	//	Sleep(MIN_SLEEP);
	//	threadCount++;		
	//}

	curl_global_init(CURL_GLOBAL_ALL);

	while (false == bThreadStarted) Sleep(MIN_SLEEP);	
	bThreadStarted = false;
	pthread_create(&threadId[threadCount], &attr[threadCount], CallCDEUrlBatch, NULL);	
	Sleep(MIN_SLEEP);
	threadCount++;		

	// Incoming SMS handler thread
	//for (int i = 0; i < 1; i++)
	//{
		while (false == bThreadStarted) Sleep(MIN_SLEEP);	
		bThreadStarted = false;
		pthread_create(&threadId[threadCount], &attr[threadCount], batchInserterIntoSmsoutboxDB, NULL);
		Sleep(MIN_SLEEP);
		threadCount++;
	//}
	
	// Incoming SMS status update handler thread
	while (false == bThreadStarted) Sleep(MIN_SLEEP);	
	bThreadStarted = false;	
	pthread_create(&threadId[threadCount], &attr[threadCount], UpdateSMSInbox, NULL);
	threadCount++;
	
	while (false == bThreadStarted) Sleep(MIN_SLEEP);	
	bThreadStarted = false;
	// Incoming SMS status update handler thread
	pthread_create(&threadId[threadCount], &attr[threadCount], UpdateFailedSMSInbox, NULL);
	threadCount++;
	

	while (false == bThreadStarted) Sleep(MIN_SLEEP);	
	bThreadStarted = false;
	// Command Server
	pthread_create(&threadId[threadCount], &attr[threadCount], runCommandServer, NULL);
	threadCount++;



	// Retry for Failed sms
	if (1 == IS_REQUE_ENABLE)
	{
		while (false == bThreadStarted) Sleep(MIN_SLEEP);	
		bThreadStarted = false;
		pthread_create(&threadId[threadCount], &attr[threadCount], RetryForFailedSMS, NULL);
		threadCount++;
	}
	
	while (false == bThreadStarted) Sleep(MIN_SLEEP);	
	bThreadStarted = false;
	// Retry for CP URL no response	
	pthread_create(&threadId[threadCount], &attr[threadCount], RetryForCPUrl, NULL);
	threadCount++;
	
	

	while(true) Sleep(3*60*1000);
	
	// Remove IPC Queue
	//RemoveIPCQ();

	curl_global_cleanup();

	return 0;
}
