#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "definition.h"
#include "inboxHandler.h"
#include "common.h"

int InitializeMsgStatus();
void getOperatorInfoFromId(int nOperatorId, char* sOperatorName);

int CallURL(char *url, char *response) {

	CURL *curl;
	CURLcode res;
	long http_code = 0;
	struct cdebuffer chunk;
	chunk.size = 0; /* no data at this point */

	int ret = -1;

	// replace new line 
	url = str_replace(url, "\r\n", "");

	memset(chunk.buf, 0, sizeof(chunk.buf));

	curl = curl_easy_init();
	if (curl) {
		// Tell libcurl the URL
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBufferCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
		res = curl_easy_perform(curl);

		if (CURLE_OK == res)
		{
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

			if (http_code == 200) 
			{
				curl_easy_cleanup(curl);
				strcpy(response, chunk.buf);
				ret = 0;
			} else 
			{
				strcpy(response, "HTTP Error");
			}
		} else 
		{
			strcpy(response, "CURL Response Error");
		}
	} else 
	{
		strcpy(response, "CURL Initialization Error");
	}

	return ret;
}


double time_diff(struct timeval x, struct timeval y) {

	/*double x_ms, y_ms, diff;

	 x_ms = (double)x.tv_sec * 1000000 + (double)x.tv_usec;
	 y_ms = (double)y.tv_sec * 1000000 + (double)y.tv_usec;

	 diff = (double)y_ms - (double)x_ms;

	 return diff;*/

	return (double) (y.tv_sec * 1000000 + y.tv_usec) - (x.tv_sec * 1000000
			+ x.tv_usec);
}

void insert_q(int no) {

	struct link *new1;

	new1 = (struct link*) malloc(sizeof(struct link));
	new1->info = no;
	new1->next = NULL;

	if (rear == NULL || front == NULL)
		front = new1;
	else
		rear->next = new1;

	rear = new1;
}

int delete_q() {

	struct link *t;
	int no = -1;

	if (front != NULL && rear != NULL) {

		t = front;
		no = t->info;
		front = front->next;
		free(t);
	}

	return (no);
}

void display() {

	struct link *t;
	t = front;

	if (front == NULL || rear == NULL)
		printf("Queue is empty\n");

	while (t != NULL) {

		printf("%d\t", t->info);
		t = t->next;
	}
}

int loadShortcode()
{
	char shortcodeURL[300], buffer[100000];

	// Hit the Service Configuration loading URL
	SPRINTF(shortcodeURL, sizeof(shortcodeURL), SHORTCODE_URL);
	
	if (0 == strlen(shortcodeURL))
	{
		return -1;
	}
	
	CallURL(shortcodeURL, buffer);

	if (0 == strlen(buffer))
	{
		return -1;
	}

	
	int i = 0;
	
	char *row, *item, *colSeperator = "|", *rowSeperator = "\r\n", *nextToken = NULL, *nextRow = NULL;
	
	item = STRTOK(buffer, colSeperator, &nextToken);	
	
	while ( NULL != item) 
	{		
		strcpy(shortcode_wise2d_queStruct[i].shortcode, item);
		shortcode_wise2d_queStruct[i].exists = true;
		
		item = STRTOK(NULL, colSeperator, &nextToken);
		i++;		
	}

	TOTAL_SHORTCODE_COUNT = i;
	
	return 0;
}


int loadShortcodeConfig()
{
	char shortcodeURL[300], buffer[100000], *src = "shortcode";

	// Hit the Service Configuration loading URL
	SPRINTF(shortcodeURL, sizeof(shortcodeURL), SHORTCODE_URL);

	if (0 == strlen(shortcodeURL))
	{
		return -1;
	}

	CallURL(shortcodeURL, buffer);

	if (0 == strlen(buffer))
	{
		return -1;
	}

	int i = 0;

	char *row, *item, *colSeperator = "|", *rowSeperator = "\r\n", *nextToken = NULL, *nextRow = NULL;

	row = STRTOK(buffer, rowSeperator, &nextRow);
	
	while (NULL != row)
	{
		item = STRTOK(row, colSeperator, &nextToken);
		if(NULL != item)
		{			
			strcpy(shortcode_wise2d_queStruct[i].shortcode, item);
		}
		
		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{						
			shortcode_wise2d_queStruct[i].tps = atoi(item);
		}

		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{	
					
			shortcode_wise2d_queStruct[i].priority = atoi(item);
		}

		row = STRTOK(NULL, rowSeperator, &nextRow);

		logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "SHORT CODE[%d]: %s | tps %d | priority %d", i, shortcode_wise2d_queStruct[i].shortcode, shortcode_wise2d_queStruct[i].tps, shortcode_wise2d_queStruct[i].priority);

		shortcode_wise2d_queStruct[i].exists = true;
		i++;
	}

	TOTAL_SHORTCODE_COUNT = i;	

	return 0;
}


int loadServiceConfig() {

/*
	Function: loadServiceConfig

	Reads all service related information and store them in local structure

	Parameters:

	fileName - Name of the configuration file.
	serviceConfig - Reference to the structure array where to store configuration information

	Returns:

	1 for success and 0 for failure.
	 */

	char serviceURL[300], buffer[100000];	

	// Hit the Service Configuration loading URL
	SPRINTF(serviceURL, sizeof(serviceURL), SERVICECONFIG_URL, SMS_CLIENT_ID);
	if (0 == strlen(serviceURL))
	{
		return -1;
	}
	CallURL(serviceURL, buffer);
	if (0 == strlen(buffer))
	{
		return -1;
	}
	
	char *row, *item, *colSeperator = "|", *rowSeperator = "\r\n", *nextToken = NULL, *nextRow = NULL;
	int reccount = 1, id = 0;

	row = STRTOK(buffer, rowSeperator, &nextRow);

	while (row != NULL){
		
		item = STRTOK(row, colSeperator, &nextToken);
		if(NULL != item)
		{	
			id = atoi(item);			
			serviceConfigStruct[id].id = id;
		}
		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{			
			strcpy(serviceConfigStruct[id].OperatorID, item);
		}
		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{			
			strcpy(serviceConfigStruct[id].Shortcode, item);
		}

		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{				
			strcpy(serviceConfigStruct[id].ServiceID, item);
		}
		
		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{			
			strcpy(serviceConfigStruct[id].SubscriptionGroupID, item);
		}
		
		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{				
			serviceConfigStruct[id].ChargeParty = atoi(item);
		}
		
		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{			
			serviceConfigStruct[id].CallCGWURL = atoi(item);
		}
		
		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{			
			serviceConfigStruct[id].TPS = atoi(item);
		}
		
		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{				
			serviceConfigStruct[id].Priority = atoi(item);
		}

		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{			
			strcpy(serviceConfigStruct[id].cgwUserID, item);
		}

		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{			
			strcpy(serviceConfigStruct[id].cgwPassword, item);
		}

		row = STRTOK(NULL, rowSeperator, &nextRow);
		
		queue2DPriorityStruct[serviceConfigStruct[id].Priority].exists = true;
		
		queue2DPriorityStruct[serviceConfigStruct[id].Priority].serviceConfigID = id;
		queue2DPriorityStruct[serviceConfigStruct[id].Priority].priority = serviceConfigStruct[id].Priority;
		

		queue2DReferenceStruct[reccount].exists = true;
		queue2DReferenceStruct[reccount].serviceConfigID = id;
	
		serviceConfigStruct[id].exists = true;		
		reccount++;
	}

	TOTAL_SERVICE_COUNT = reccount - 1;
	return 0;
}


void getServiceInfo(const char* ServiceID, int* CallCGWURL)
{  
	for (int i = 1; i <= TOTAL_SERVICE_COUNT; i++)
	{			
		if (true == serviceConfigStruct[i].exists	&&
			(0 == strcmp(serviceConfigStruct[i].ServiceID, ServiceID)) ||
			(0 == strcmp(serviceConfigStruct[i].SubscriptionGroupID, ServiceID)))
		{		
			*CallCGWURL = serviceConfigStruct[i].CallCGWURL;			
			break;
		}
	}
}

int InitializeMsgStatus() {

	/*
	 Function: InitializeMsgStatus

	 Update all SMS status to QUE which have status PROCESSING

	 Returns:

	 1 for success and -1 for failure.
	 */

	MYSQL *connection = NULL;

	int ret = ConnectToDatabase(connection, 0, "InitializeMsgStatus",
			"Re-Queueing");
	if(1 == ret) 
	{
		logActivity(__FUNCTION__, "Initialize", E_ERROR, "Connected to database");
	}
	

	if (-1 == ret)
		return -1;

	char query[MAXQUERYLENGTH];

	SPRINTF(query, sizeof(query),
			"UPDATE smsoutbox SET msgStatus = 'QUE' WHERE msgStatus = 'PROCESSING'");

	if (mysql_query(connection, query)) {

		Finish_with_error(connection,"initializeMsgStatus");
		return -1;
	}

	mysql_close(connection);

	return 1;
}

 bool emptyInboxQueue(pInbox_queue *queue)
 {
	 //if((*queue)->Head !=NULL)
	 //logActivity(__FUNCTION__, "emptyck", E_DEBUG_MAJOR, "smsid: %d", (*queue)->Head->sms.msgID);
 	return (*queue)->Head == NULL ? true : false;
 }

 bool emptyOutboxQueue(pOutbox_queue *queue)
 {
 	return (*queue)->Head == NULL ? true : false;
 }

pOutbox_queue createOutboxQueue()
{
	pOutbox_queue Queue;

	if ( (Queue = (pOutbox_queue) malloc(sizeof(Queue))) == NULL)
		return (NULL);
	Queue->Head = NULL;
	Queue->Tail = NULL;

	return (Queue);
}

pInbox_queue createInboxQueue() {

	pInbox_queue Queue;

	if ((Queue = (pInbox_queue) malloc(sizeof(Queue))) == NULL)
		return (NULL);
	Queue->Head = NULL;
	Queue->Tail = NULL;

	return (Queue);
}

void deleteInboxQueue(pInbox_queue queue) 
{	
	pInbox_queue tmpQueue;

	while (queue != NULL && queue->Head != NULL)
	{
		tmpQueue = queue;
		//memcpy(smsEntry, &(queue->Head->sms), sizeof(queue->Head->sms));

		if (queue->Head->next != NULL)
			queue->Head = queue->Head->next;
		else
			queue->Head = NULL;
		
		free(tmpQueue);
	}
}

pInbox_queue createInboxResponseQueue() {

	pInbox_queue Queue;

	if ((Queue = (pInbox_queue) malloc(sizeof(Queue))) == NULL)
		return (NULL);
	Queue->Head = NULL;
	Queue->Tail = NULL;

	return (Queue);
}

//void dequeue_smsinbox(pInbox_queue queue, smsinbox *smsEntry) 
//{
//
//	if (queue != NULL && queue->Head != NULL)
//	{
//		//logActivity(__FUNCTION__, "insDQue", E_ERROR, "MsgID: %d", queue->Head->sms.msgID);
//		memcpy(smsEntry, &(queue->Head->sms), sizeof(queue->Head->sms));
//
//		if (queue->Head->next != NULL)
//			queue->Head = queue->Head->next;
//		else
//			queue->Head = NULL;
//	}
//}


void dequeue_smsinbox(pInbox_queue queue, smsinbox *smsEntry)
{

	pInbox_node tmp = NULL;

	if ((queue)->Head != NULL)
	{
		tmp = queue->Head;
		memcpy(smsEntry, &(queue->Head->sms), sizeof(queue->Head->sms));

		pthread_mutex_lock(&queueLock_flag);
		(queue)->Head = (queue)->Head->next;

		free(tmp);

		if ((queue)->Head == NULL)
			(queue)->Tail = NULL;

		pthread_mutex_unlock(&queueLock_flag);
	}
	else
	{
		//logActivity(__FUNCTION__, "QNULL", E_ERROR, "head is null");
		return;
	}
}

//void enqueue_smsinbox(pInbox_queue *queue, smsinbox smsEntry) {
//
//	pInbox_node pnew = NULL;
//
//	if (*queue != NULL) {
//
//		if ((pnew = (pInbox_node)malloc(sizeof(inbox_node))) != NULL) {
//
//			pnew->next = NULL;
//
//			memcpy(&(pnew->sms), &smsEntry, sizeof(smsEntry));
//
//			pthread_mutex_lock(&queueLock_flag);
//			if (emptyInboxQueue(queue))
//			{
//				(*queue)->Head = pnew;
//				pthread_mutex_unlock(&queueLock_flag);
//				return;
//			}
//			(*queue)->Tail->next = pnew;
//			(*queue)->Tail = pnew;
//			pthread_mutex_unlock(&queueLock_flag);
//		}
//	}
//}


void enqueue_smsinbox(pInbox_queue *queue, smsinbox smsEntry) {

	pInbox_node pnew = NULL;

	if (*queue != NULL) {

		if ((pnew = (pInbox_node)malloc(sizeof(inbox_node))) != NULL) {

			pnew->next = NULL;

			memcpy(&(pnew->sms), &smsEntry, sizeof(smsEntry));

			pthread_mutex_lock(&queueLock_flag);
			//if (emptyInboxQueue(queue))
			if ((*queue)->Tail == NULL)
			{
				(*queue)->Head = (*queue)->Tail = pnew;
				pthread_mutex_unlock(&queueLock_flag);
				return;
			}
			(*queue)->Tail->next = pnew;
			(*queue)->Tail = pnew;
			pthread_mutex_unlock(&queueLock_flag);
		}
	}
}

void makeSMSBatch(int msgID, char* strBatch)
{
	if(strlen(strBatch) > 0)
	{
		strcat(strBatch, ",");
	}
	
	char sMsgID[20];
	SPRINTF(sMsgID, sizeof(sMsgID), "%d", msgID);
	strcat(strBatch, sMsgID);
}

//int pickCurrentInboxShortcodes()
//{
//	char query[MAXQUERYLENGTH], *src = "CurrInbx", msgIDs[200], itoabuffer[19];
//	int ret, batchSize = 0, MAX_BATCH_SIZE = 20, rowCounter = 0, tpsMultiplierFactor = 5, i;
//	MYSQL *con = NULL;
//	MYSQL_RES *result;
//	MYSQL_ROW row;
//	struct timeval before, after;
//	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;
//	int bDBConnected = 0;
//
//	if (0 == bDBConnected)
//	{
//		bDBConnected = ConnectToDatabase(con, 0, src, "Fetch row from smsinbox");
//	}
//
//	if (1 == bDBConnected)
//	{
//		//logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "Connected to database");
//
//		SPRINTF(query, sizeof(query), "SELECT DISTINCT dstMN FROM smsinbox where msgStatus = 'QUE'");
//
//		//logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "SMSINBOX QUERY: %s", query);
//
//		ret = 1;
//
//		if (NULL != con && strlen(query) > 0)
//		{
//			ret = mysql_query(con, query);
//		}
//
//		if (0 != ret)
//		{
//			logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY: %s, RESULT : %d , error no: %d", query, ret, mysql_errno(con));
//			if (NULL != con)
//			{
//				Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
//				bDBConnected = 0;
//				Sleep(1000);
//				return -1;
//				//break;
//			}
//		}
//
//		result = mysql_store_result(con);
//
//		if (NULL == result)
//		{
//			logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY: %s, mysql_store_result NULL", query);
//
//			if (NULL != con)
//			{
//				Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
//				bDBConnected = 0;
//				Sleep(1000);
//				return -1;
//				//break;
//			}
//		}
//
//		QUED_SHORTCODE_COUNT = mysql_num_rows(result);
//
//		logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "SMSINBOX QUERY: %s|Shortcode count: %d", query, QUED_SHORTCODE_COUNT);
//
//		i = 0;
//
//		while ((row = mysql_fetch_row(result)) != false)
//		{
//			strcpy(queuedSmsinboxShortcodes[i].shortcode, row[0]);
//			logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " Shortcode: %s", queuedSmsinboxShortcodes[i].shortcode);
//			i++;
//		}
//
//		if (NULL != result)
//		{
//			mysql_free_result(result);
//			result = NULL;
//		}
//
//		if (NULL != con)
//		{
//			mysql_close(con);
//			con = NULL;
//		}
//
//	}
//
//	return 1;
//}

//new code

void pickCurrentInboxShortcodes()
{
	char query[MAXQUERYLENGTH], *src = "CurrInbx", msgIDs[200], itoabuffer[19];
	int ret, batchSize = 0, MAX_BATCH_SIZE = 20, rowCounter = 0, tpsMultiplierFactor = 5, i;
	MYSQL *con = NULL;
	MYSQL_RES *result;
	MYSQL_ROW row;
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;
	int bDBConnected = 0;

	while (1)
	{
		if (ConnectToDatabase(con, 0, src, "Fetch row from smsinbox"))
		{
			sprintf(query, "SELECT DISTINCT dstMN FROM smsinbox where msgStatus = 'QUE'");

			if (mysql_query(con, query) == 0)
			{
				result = mysql_store_result(con);

				QUED_SHORTCODE_COUNT = mysql_num_rows(result);

				i = 0;
				while (row = mysql_fetch_row(result))
				{
					//add2
					strcpy(queuedSmsinboxShortcodes[i++].shortcode, row[0]);
				}

				logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "SMSINBOX QUERY: %s|Shortcode count: %d", query, QUED_SHORTCODE_COUNT);
			}
			else
			{
				logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY: %s, RESULT : %d , error no: %d", query, ret, mysql_errno(con));
			}

			mysql_free_result(result);
			result = NULL;
			closeConnection(src, con);
			break;
		}
		else
		{
			logActivity(__FUNCTION__, src, E_ERROR, "connection failed in pickCurrentInboxShortcodes function");
			Sleep(10);
		}
	}
}

void *PickQUEDSMSFromInbox(void *p) 
{
	MYSQL *con = NULL;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	char query[MAXQUERYLENGTH];
	smsinbox entry;
	int ret;
	char *src = "DB2QInbox";
	char strBatch[10000] = {0};
	char fiveDigitSC[10] = { 0 };
	int MAX_BATCH_SIZE = 20;
	int currentBatchSize = 0, tpsMultiplierFactor = 5;
	int bDBConnected = 0;		
	int isPickedCurrentInboxShortcodes = 0;

	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;
	bool isSleepRequired;
	bThreadStarted = true;
	logActivity(__FUNCTION__, src, E_ERROR, "PickQUEDSMSFromInbox thread started");

	while (true)
	{
		if (0 == bDBConnected)
		{
			bDBConnected = ConnectToDatabase(con, 0, src, "Fetch row from smsinbox");
			if (1 == bDBConnected)
			{
				logActivity(__FUNCTION__, src, E_ERROR, "Connected to database");
			}
		}

		// if db connected
		if (1 == bDBConnected ) 
		{	
			while (true)
			{
				isPickedCurrentInboxShortcodes = 0;
				
				// check current shortcodes which have qued sms
			//isPickedCurrentInboxShortcodes = pickCurrentInboxShortcodes();  //bad coding

				pickCurrentInboxShortcodes();  //bad coding

				

				//if (1 != isPickedCurrentInboxShortcodes)
				//{
				//	logActivity(__FUNCTION__, src, E_ERROR, "Failed to pick shortcodes from smsinbox");
				//	continue;
				//}

				

				if(false == isPingSent || isInboxSingleMemoryQueOverloaded() || true == bReloadingShortcode || false == bReloadComplete)
				{					
					Sleep(MIN_SLEEP);
					continue;
				}

				//logActivity(__FUNCTION__,"afterPick", E_DEBUG_MAJOR, "QUED_SHORTCODE_COUNT: %d,TOTAL_SHORTCODE_COUNT: %d", QUED_SHORTCODE_COUNT, TOTAL_SHORTCODE_COUNT);

				isSleepRequired = true;

				for (int j = 0; j < QUED_SHORTCODE_COUNT; j++)
				{
					for (int i = 0; i < TOTAL_SHORTCODE_COUNT && 1 == bDBConnected ; i++)   ///// extra code
					{
						if (queuedSmsinboxShortcodes[j].shortcode)
						{

							if (strlen(queuedSmsinboxShortcodes[j].shortcode) == 4 && APPEND_SHORTCODE_PREFIX)
							{
								sprintf(fiveDigitSC, "%d%s", SHORTCODE_PREFIX_VALUE, queuedSmsinboxShortcodes[j].shortcode);
							}
							else
							{
								sprintf(fiveDigitSC, queuedSmsinboxShortcodes[j].shortcode);
							}
						}


						//logActivity(__FUNCTION__, "mytestt", E_DEBUG_MAJOR, "five_shortcode:<%s>", fiveDigitSC);

						if (true == shortcode_wise2d_queStruct[i].exists &&
							0 == strcmp(shortcode_wise2d_queStruct[i].shortcode, fiveDigitSC))
						{
							//already many sms are in processing state
							while(COUNT_SMS_IN_APP_QUE > MAX_SMS_IN_QUE )
							{
								Sleep(MIN_SLEEP);
							}

							// Take initial timestamp
							gettimeofday(&before, NULL);
							// Pick a constant number (say 1000, that will be put as LIMIT value in SQL query) of SMS with state = QUE for i-th service	from DB
							SPRINTF(query,	sizeof(query),
								"SELECT * FROM smsinbox WHERE dstmn = '%s' AND msgStatus = 'QUE' LIMIT %d",
								queuedSmsinboxShortcodes[j].shortcode, /*shortcode_wise2d_queStruct[i].tps*/5*tpsMultiplierFactor);   //wrong shortcode_wise2d_queStruct[i].tps
							logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "INBOX FETCH QUERY: %s | i:%d", query,i);
							ret = 1;

							// Take final timestamp
							gettimeofday(&after, NULL);
							timeDiff = time_diff(before, after) / 1000;

							logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "picking qued SMS time diff: %f", timeDiff);

							ret = mysql_query(con, query);
							result = mysql_store_result(con);

							if (NULL == result)
							{				
								logActivity(__FUNCTION__, src, E_ERROR, "Failed Query : %s, RESULT: %d", query, ret);
								if (NULL != con)
								{
									Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
									bDBConnected = 0;
									Sleep(1000);
									break;														
								}						
							}

							while ( (NULL != result) &&(row = mysql_fetch_row(result)))
							{
								strcpy(entry.srcMN, row[0]);
								if (row[1] && strlen(row[1]) == 4 && APPEND_SHORTCODE_PREFIX)
								{
									sprintf(entry.dstMN, "%d%s", SHORTCODE_PREFIX_VALUE, row[1]);
								}
								else
								{
									strcpy(entry.dstMN, row[1]);
								}
								
								strcpy(entry.msg, row[2]);
								entry.msgID = atoi(row[4]);
								entry.retryCount = atoi(row[6]);

								if(row[7] != NULL && strlen(row[7]) > 0)
								{
									strcpy(entry.refID, row[7]);
								}
								else
								{
									strcpy(entry.refID, row[4]); // use msgid as refID
								}

								entry.channel = atoi(row[8]);
								if (NULL != row[9])
								{
									strcpy(entry.srcAccount,row[9]);
								}							

								// Push the SMS in queue dedicated for this service in serviceQueueArray[] array
								enqueue_smsinbox(&shortcodeQueueArray[i], entry);
								COUNT_SMS_IN_APP_QUE++;

								// Update msg status, in a batch for faster 

								makeSMSBatch(entry.msgID, strBatch);
								currentBatchSize++;
								if(MAX_BATCH_SIZE == currentBatchSize)
								{
									// Take initial timestamp
									gettimeofday(&before, NULL);

									SPRINTF(query, sizeof(query),"UPDATE smsinbox SET msgStatus = 'PROCESSING' WHERE msgID in (%s)", strBatch);
									logActivity(__FUNCTION__, src, E_ERROR, "MsgID Batch: %s", strBatch);
									memset(strBatch, 0, sizeof(strBatch));
									currentBatchSize = 0;									

									logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " UPDATING STATUS AS PROCESSING, QUERY: %s", query);
									ret = 1;
									// Take final timestamp
									gettimeofday(&after, NULL);
									timeDiff = time_diff(before, after) / 1000;

									logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "batchsize greater than max time diff: %f", timeDiff);

									ret = mysql_query(con, query);	
									if (0 != ret)
									{
										logActivity(__FUNCTION__, src, E_ERROR, " Failed Query : %s, RESULT: %d", query, ret);
										if (NULL != con)
										{
											if (NULL != result)
											{
												mysql_free_result(result);
												result = NULL;
											}
											Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
											bDBConnected = 0;
											Sleep(1000);
											break;										
										}
									}
								}				

								if (isSleepRequired)
									isSleepRequired = false;

								//Sleep(1000);
							}
							if(currentBatchSize > 0)
							{
								// Take initial timestamp
								gettimeofday(&before, NULL);

								SPRINTF(query, sizeof(query),"UPDATE smsinbox SET msgStatus = 'PROCESSING' WHERE msgID in (%s)", strBatch);
								logActivity(__FUNCTION__, src, E_ERROR, "MsgID Batch: %s", strBatch);
								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " UPDATING STATUS AS PROCESSING, QUERY: %s | num SMS: %d", query, COUNT_SMS_IN_APP_QUE);
								memset(strBatch, 0, sizeof(strBatch));
								currentBatchSize = 0;

								ret = 1;

								// Take final timestamp
								gettimeofday(&after, NULL);
								timeDiff = time_diff(before, after) / 1000;

								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "batchsize less than max time diff: %f", timeDiff);

								ret = mysql_query(con, query);	

								// Query failed
								if (0 != ret)
								{				
									if (NULL != result)
									{
										mysql_free_result(result);
										result = NULL;
									}
									logActivity(__FUNCTION__, src, E_ERROR, " Failed Query : %s, RESULT: %d", query, ret);
									if (NULL != con)
									{
										Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
										bDBConnected = 0;
										Sleep(1000);
										break;																
									}					
								}
							}

							if(NULL != result)
							{
								mysql_free_result(result);
								result = NULL;
							}
						}

						if (isSleepRequired)
							Sleep(MIN_SLEEP);
					}
				}
				

				// break from while and connect db again
				if (0 == bDBConnected)
				{
					break;
				}
				Sleep(100);
			}
		} 
		else 
		{
			logActivity(__FUNCTION__, src, E_ERROR, " DB CONNECTION FAILED");	
			bDBConnected = 0;
			Sleep(1000);
		}

		Sleep(MIN_SLEEP);
	}	

	return NULL;
}

void *PickShortcodeWiseSMSFromQueue(void *p) {

	struct smsinbox entry;
	int smsCount,i;
	bThreadStarted = true;
	bool isSleepRequired;
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;
	int threadNo;
	threadNo = *((int*)p);
	char *src = "InbxIpcQ";

	logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "PickShortcodeWiseSMSFromQueue, TOTAL_SHORTCODE_COUNT: %d", TOTAL_SHORTCODE_COUNT);
	
	while (true)
	{		
		if( true == isInboxSingleMemoryQueOverloaded() || true == bReloadingShortcode)
		{
			logActivity(__FUNCTION__, "shuvo", E_DEBUG_MAJOR, "inseide isInboxSingleMemoryQueOverloaded");
			Sleep(MIN_SLEEP);
			continue;
		}

		isSleepRequired = true;
		for (i = 0; i < TOTAL_SHORTCODE_COUNT; i++) ///
		{
			// service is reloading, so wait
			while (true == bReloadingShortcode)
			{
				logActivity(__FUNCTION__, "shuvo", E_DEBUG_MAJOR, "inseide bReloadingShortcode");
				Sleep(MIN_SLEEP);
			}

			if (true == shortcode_wise2d_queStruct[i].exists)
			{
				//logActivity(__FUNCTION__, "shuvo", E_DEBUG_MAJOR, "inseide shortcode_wise2d_queStruct[i].exists,i:%d",i);
				// Pick a short code -specific constant number (according to priority assigned) of SMS from queue
				smsCount = 100; //SHORTCODE_TPS;

				while (smsCount > 0)   //need to modify
				{
					//logActivity(__FUNCTION__, "tapas", E_ERROR, "inseide while loop, i: %d", i);
					if (!emptyInboxQueue(&shortcodeQueueArray[i]) )  //0
					{
						/*while (bSmsDequeueing)
							Sleep(MIN_SLEEP);*/

						// Take initial timestamp
						gettimeofday(&before, NULL);

						//bSmsDequeueing = true;

						//logActivity(__FUNCTION__, "shuvo", E_DEBUG_MAJOR, "inside function................");

						dequeue_smsinbox(shortcodeQueueArray[i], &entry);
						COUNT_SMS_IN_APP_QUE--;

						logActivity(__FUNCTION__, "PUSHinbxQ", E_DEBUG_MAJOR, " PickShortcodeWiseSMSFromQueue[%d] MessageID: %d", threadNo,entry.msgID);

						
						logActivity(__FUNCTION__, "QUE", E_DEBUG_MAJOR, " SrcMN: %s  Message: %s MessageID: %d",entry.srcMN, entry.msg,entry.msgID);
						
						// Take final timestamp
						gettimeofday(&after, NULL);
						timeDiff = time_diff(before, after) / 1000;
						logActivity(__FUNCTION__, "PUSHinbxQ", E_DEBUG_MINOR, "deque timediff %f PickShortcodeWiseSMSFromQueue[%d] MessageID: %d", timeDiff, threadNo, entry.msgID);
						
						// Take initial timestamp
						gettimeofday(&before, NULL);
						
						// insert into memory que
						insertIntoInboxSingleMemoryQue(entry);

						// Take final timestamp
						gettimeofday(&after, NULL);
						timeDiff = time_diff(before, after) / 1000;	
						logActivity(__FUNCTION__, "IPCQentr", E_DEBUG_MINOR, "time diff %f, msgIDS : %d",timeDiff, entry.msgID);

						if (isSleepRequired)
							isSleepRequired = false;
					} 
					else 
					{
						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "empty queue (shortcodeQueueArray[i])");
						break;   //need to modify
					}

					smsCount--;
				}
			}

			if (isSleepRequired)
				Sleep(MIN_SLEEP);
		}
		Sleep(MIN_SLEEP);
	}

	return NULL;
}


static void addToCurlMulti(CURLM *cm, int index, const char* url, smsinbox sms)
{		
	int pos =  index;

	logActivity(__FUNCTION__, "in_cmah", E_DEBUG_MINOR, "index: %d, pos: %d, msgId: %d, url %s", index, pos, sms.msgID, url);

	memset(&chunkResponse[pos], MEMINIT_VALUE, sizeof(struct httpResponseBuf));
	
	struct index_msgId_map *dataMap = (struct index_msgId_map *)calloc(1, sizeof(struct index_msgId_map));
	dataMap->index = pos;	
	dataMap->sms = sms;

	CURL *eh = curl_easy_init();
	curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(eh, CURLOPT_WRITEDATA, &chunkResponse[pos]);
	curl_easy_setopt(eh, CURLOPT_HEADER, 0L);
	curl_easy_setopt(eh, CURLOPT_TIMEOUT, RETRY_DELAY);
	curl_easy_setopt(eh, CURLOPT_URL, url);
	curl_easy_setopt(eh, CURLOPT_PRIVATE, dataMap);
	curl_easy_setopt(eh, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(eh, CURLOPT_NOSIGNAL, 1L);	

	CURLMcode ret = curl_multi_add_handle(cm, eh);

	if (CURLM_OK != ret)
	{
		logActivity(__FUNCTION__, "in_cmah", E_ERROR, "curl_multi_add_handle Error: %d, index: %d, pos: %d, msgId: %d", ret, index, pos, sms.msgID);
	}	  
}

void HandleHTTPResponse(CURLM* cm)
{
	char* src = "inurlrsp";
	MYSQL *con = NULL;
	CURL *eh = NULL;
	CURLMsg *curlMsg = NULL;
	CURLcode curl_code;
	int still_running=0, i=0, msgs_left=0;
	struct smsoutbox entry;
	struct smsinbox sms;
	struct cde_failed cde;
	int http_code;	
	int msgID = 0;	
	int isMsgSent = 0;
	int isDlrCallbackEnabled = 0;
	int nServiceConfigId, nCallChargingUrlFlag = 0;
	int index = 0;	
	char  query[2000], StatusText[20], Remarks[500];
	char mainMsg[1000], serviceId[500];
	bool bDBConnected = false;	
	struct index_msgId_map* pDataMap;	
	int ret = 0;	
	int pos;
	int nErr;
	char *token;
	bool noreply;
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;	

	logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "CDE URL Response started");

	while (curlMsg = curl_multi_info_read(cm, &msgs_left)) 
	{		
		if (CURLMSG_DONE == curlMsg->msg) 
		{				
			eh = curlMsg->easy_handle;
			if (NULL == eh)
			{
				continue;
			}				

			// Get HTTP status code
			http_code = 0;
			pDataMap = NULL;						
			memset(StatusText, MEMINIT_VALUE, sizeof(StatusText));
			memset(Remarks,  MEMINIT_VALUE, sizeof(Remarks));
			isMsgSent = 0;
			pos = index;

			curl_easy_getinfo(eh, CURLINFO_RESPONSE_CODE, &http_code);
			curl_easy_getinfo(eh, CURLINFO_PRIVATE, &pDataMap);	

			if (NULL != pDataMap)
			{
				index = pDataMap->index;
				//sms.msgID = pDataMap->sms.msgID;				
				sms = pDataMap->sms;
			}

			logActivity(__FUNCTION__, src, E_DEBUG_MINOR, "CDE URL Response | msgID: %d, http_code: %d", sms.msgID, http_code);

			curl_code = curlMsg->data.result;

			if(CURLE_OK == curl_code) 
			{
				if (200 == http_code)
				{					
					logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "CDE RESPONSE: %s", chunkResponse[index].buf);

					token = "";
					token = strtok(chunkResponse[index].buf, "|");
					if (token != NULL)
					{
						strcpy(mainMsg, token);
						noreply = false;
						for (int i = 0; i < NOREPLY_STRING_NUMBER; i++)
						{
							if (strncmp(mainMsg, NOREPLY_STRINGS[i], strlen(NOREPLY_STRINGS[i])) == 0)
							{
								noreply = true;
								break;
							}
						}

						token = strtok(NULL, "|");

						// get service id
						SPRINTF(serviceId, sizeof(serviceId), "%s",	token == NULL ? "" : token);

						// Now We need to get IsChargingEnable and ServiceConfigId							
						getServiceInfo(serviceId, &nCallChargingUrlFlag);	

						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "MESSAGE:%s | SERVICE ID: %s", mainMsg, serviceId);
						// fill up the struct
						strcpy(entry.serviceID, serviceId);
						strcpy(entry.srcMN, sms.dstMN);
						strcpy(entry.dstMN, sms.srcMN);
						strcpy(entry.msg, mainMsg);
						strcpy(entry.msgType, "text"); // where it comes from

						entry.channel = sms.channel;
						if(NULL != sms.srcAccount)
						{
							strcpy(entry.destAccount, sms.srcAccount);
						}
						else
						{
							strcpy(entry.destAccount, "");
						}

						if(NULL != sms.refID && strlen(sms.refID) > 0)
						{
							strcpy(entry.refID, sms.refID);
						}
						else
						{								
							SPRINTF(entry.refID, sizeof(entry.refID), "%d",	sms.msgID);								
						}

						entry.isChargingEnabled = nCallChargingUrlFlag;							

						// If NOREPLY_STRINGS found, no reply sms will be inserted to smsoutbox
						if( true == noreply)
						{
							logActivity(__FUNCTION__, src, E_ERROR, "  MESSAGE: %s | MESSAGE ID: %d", entry.msg, sms.msgID);
						}

						if (false == noreply)
						{
							// Insert into smsQueue							
							logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " INSERTED INTO SMS OUTBOX QUEUE | SOURCE: %s | DESTINATION: %s | MESSAGE: %s | MESSAGE ID: %d", entry.srcMN, entry.dstMN, entry.msg, msgID);

							insertIntoOutboxSingleMemoryQue(entry);
						}

						// insert into inbox response que
						insertIntoInboxResponseMemoryQue(sms);
					}
					else
					{
						//logActivity(__FUNCTION__, src, E_ERROR, " CDE Response: No content, MESSAGE: %s, MESSAGE ID: %d, CDE url: %s", sms.msg, sms.msgID, urltmp);
						logActivity(__FUNCTION__, src, E_ERROR, " CDE Response: No content, MESSAGE: %s, MESSAGE ID: %d", sms.msg, sms.msgID);
						cde.msgID = sms.msgID;
						insertIntoCDEFailedMemoryQue(cde);
					}
				}
				else
				{						
					logActivity(__FUNCTION__, src, E_ERROR, "CDE URL FAILED, http_code: %d", http_code);
					//for failed sms
					cde.msgID = sms.msgID;
					insertIntoCDEFailedMemoryQue(cde);
				}
			}
			else
			{
				logActivity(__FUNCTION__, src, E_ERROR, "CURL FAILED, MESSAGE ID: %d, CURLcode: %d", sms.msgID, curl_code);
				//for failed sms
				cde.msgID = sms.msgID;
				insertIntoCDEFailedMemoryQue(cde);								

			} // end if(CURLE_OK == return_code) 
			

			if (NULL != pDataMap)
			{
				free(pDataMap);
			}

			curl_multi_remove_handle(cm, eh);
			curl_easy_cleanup(eh);				
		} // end if (CURLMSG_DONE == curlMsg->msg) 
		else 
		{			
			logActivity(__FUNCTION__, src, E_ERROR, "CDE URL CURL Response : %d", curlMsg->msg);
		}		
	}	
}

void *CallCDEUrlBatch(void *p)
{	
	long http_code = 0;
	CURLM *cm = NULL; //curl multi
	CURL *curl;
	CURLcode res;	
	char mainMsg[1000], url[URL_SIZE], serviceId[500];	
	struct smsoutbox entry;
	struct smsinbox sms;
	struct cde_failed cde;	
	char *SMSGW_CDE_URL = CDE_URL;
	int cdethread;
	char *src = "CallCDEURL";
	char dest[500] = {0};
	int nCallChargingUrlFlag = 0;
	int nOperatorID = 0;
	int nret;
	char operatorName[20] = {0};
	char *token;
	bThreadStarted = true;
	bool noreply;
	int currentBatchSize = 0;
	char strBatchUrl[CURL_BATCH_SIZE][URL_SIZE] = {0};	
	int still_running;
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;


	logActivity(__FUNCTION__, src, E_ERROR, "CallCDEUrlBatch thread started");

	while (true)
	{
		// curl multi init				
		if (NULL == cm)
		{			
			cm = curl_multi_init();	
		}
		
		// if any of these que is overloaded, CDE url will not be called.
		if(isOutboxSingleMemoryQueOverloaded() || isCDEFailedMemoryQueOverloaded() || isInboxResponseMemoryQueOverloaded())	
		{
			logActivity(__FUNCTION__, src, E_ERROR, "delay: ipcq overflow");
			Sleep(1000);
			continue;
		}

		// Take initial timestamp
		gettimeofday(&before, NULL);
		
		nret = getFromInboxSingleMemoryQueue(&sms);		

		logActivity(__FUNCTION__, "POPInbxQ", E_DEBUG_MAJOR, "MsgID: %d", sms.msgID);	
		
		// Pick a SMS from cde queue
		if (-1 != nret)
		{
			logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " SMS_INBOX_QUE_SHMID ret: %d, SrcMN :%s, Message: %s MessageID: %d", nret, sms.srcMN, sms.msg,sms.msgID);

			if (0 == sms.msgID)
			{
				logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Found in single inbox queue");

				//insert Ping SMS into cde failed que
				cde.msgID = 0;
				insertIntoCDEFailedMemoryQue(cde);
				logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Pushed in CDE FAILED queue");

				//insert Ping SMS into inbox Response que
				sms.msgID = 0;
				insertIntoInboxResponseMemoryQue(sms);
				logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Pushed in INBOX RESPONSE queue");

				strcpy(entry.msgType, "PINGSMS");
				strcpy(entry.refID, "PINGSMS");
				Sleep(1000);
				insertIntoOutboxSingleMemoryQue(entry);
				logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Pushed in OUTBOX queue");
				continue;
			} /////
			//
			nCallChargingUrlFlag = 0;

			// Now i have to enter correct msg to queue for entry of smsoutbox					

			// message urlencode
			urlencode(dest, sms.msg);    

			nOperatorID = atoi(sms.srcAccount);			
			getOperatorInfoFromId(nOperatorID, operatorName);
			if (0 == strlen(operatorName) ||
				0 == strlen(sms.srcMN) ||
				0 == strlen(sms.dstMN))
			{
				logActivity(__FUNCTION__, src, E_ERROR, " Operator Name: %s", operatorName);
				cde.msgID = sms.msgID;
				insertIntoCDEFailedMemoryQue(cde);
				continue;
			}			

			// if operator configuration is wrong
			if (NULL == operatorConfigStruct[nOperatorID].operatorName || 
				0 == strlen(operatorConfigStruct[nOperatorID].operatorName))
			{
				logActivity(__FUNCTION__, src, E_ERROR, "SourceAccount configuration wrong :  %s", sms.msgID);
				cde.msgID = sms.msgID;
				insertIntoCDEFailedMemoryQue(cde);
				continue;
			}
			memset(url, MEMINIT_VALUE, sizeof(url));
			/*CDE_URL->http://localhost/smsgw_ws/core/cde.php?mn=%s&msg=%s&sc=%s&rempc=%s&op=%s&tid=%s&retry=%d */
			SPRINTF(url, sizeof(url), SMSGW_CDE_URL, sms.srcMN, dest, sms.dstMN, "Y", operatorConfigStruct[nOperatorID].operatorName, sms.refID, sms.retryCount);

			// replace new line 
			char* urltmp = str_replace(url, "\r\n", "");
			memset(strBatchUrl[currentBatchSize], MEMINIT_VALUE, URL_SIZE*sizeof(char));
			strcpy(strBatchUrl[currentBatchSize], urltmp);//
			logActivity(__FUNCTION__, "myURL", E_DEBUG_MAJOR, " CDE URL: %s", strBatchUrl[currentBatchSize]);

			//now we have to add to multi curl
			addToCurlMulti(cm, currentBatchSize, strBatchUrl[currentBatchSize], sms);						
			currentBatchSize++;	

			if (currentBatchSize >= CDE_CURL_BATCH_SIZE)
			{					
				still_running = 0;				
				currentBatchSize = 0;

				curl_multi_perform(cm, &still_running);

				do 
				{
					int numfds=0;
					curl_multi_perform(cm, &still_running);								

				} while(still_running);								

				HandleHTTPResponse(cm);
				memset(strBatchUrl, MEMINIT_VALUE, sizeof(strBatchUrl));				
			}			
		}  /////////////////////////////////////////////////
		else  
		{
			logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "No sms in ipcq, currentBatchSize: %d", currentBatchSize); 
			if (currentBatchSize > 0 && currentBatchSize < CDE_CURL_BATCH_SIZE)
			{					
				still_running = 0;				
				currentBatchSize = 0;
				curl_multi_perform(cm, &still_running);

				do 
				{
					int numfds=0;
					curl_multi_perform(cm, &still_running);								

				} while(still_running);							

				HandleHTTPResponse(cm);
				memset(strBatchUrl, MEMINIT_VALUE, sizeof(strBatchUrl));
			}
			else
			{
				Sleep(MIN_SLEEP);
			}
		}		
	}

	return NULL;
}

void *CallCDEURL(void *p)
{	
	long http_code = 0;
	CURL *curl;
	CURLcode res;	
	char mainMsg[1000], url[1000], srcAccount[50], serviceId[500];
	memset(srcAccount, MEMINIT_VALUE, sizeof(srcAccount));
	struct smsoutbox entry;
	struct smsinbox sms;
	struct cde_failed cde;
	struct cdebuffer chunk;
	char *SMPP_CDE_URL = CDE_URL;
	int cdethread;
	
	char *src = "CallCDEURL";
	char dest[50] = {0};
	int nServiceConfigId, nCallChargingUrlFlag = 0;
	int nOperatorID = 0;
	int nret;
	char operatorName[20] = {0};
	char *token;
	bThreadStarted = true;
	bool noreply;

	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;
	
	cdethread = *((int*)p);
	//logActivity(__FUNCTION__, src, E_ERROR, "cdethread %d", cdethread);
	
	while (true)
	{
		// if any of these que is overloaded, CDE url will not be called.
		if(isOutboxSingleMemoryQueOverloaded() || isCDEFailedMemoryQueOverloaded() || isInboxResponseMemoryQueOverloaded())	
		{
			logActivity(__FUNCTION__, src, E_ERROR, " delay que over flowed");
			Sleep(MIN_SLEEP);
			continue;
		}	
		while (bSmsReceivingFromQue)
		{
			Sleep(MIN_SLEEP);
		}
		bSmsReceivingFromQue = true;

		// Take initial timestamp
		gettimeofday(&before, NULL);

		nret = getFromInboxSingleMemoryQueue(&sms);

		logActivity(__FUNCTION__, "POPInbxQ", E_ERROR, "cdethread[%d] MsgID: %d", cdethread,sms.msgID);

		logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " SMS_INBOX_QUE_SHMID ret: %d, SrcMN :%s, Message: %s MessageID: %d", nret, sms.srcMN, sms.msg,sms.msgID);

		bSmsReceivingFromQue = false;
		// Pick a SMS from cde queue
		if (-1 != nret)
		{
			if (0 == sms.msgID)
			{
				logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Found in single inbox queue");
				
				//insert Ping SMS into cde failed que
				cde.msgID = 0;
				insertIntoCDEFailedMemoryQue(cde);
				logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Pushed in CDE FAILED queue");

				//insert Ping SMS into inbox Response que
				sms.msgID = 0;
				insertIntoInboxResponseMemoryQue(sms);
				logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Pushed in INBOX RESPONSE queue");

				strcpy(entry.msgType, "PINGSMS");
				strcpy(entry.refID, "PINGSMS");
				Sleep(1000);
				insertIntoOutboxSingleMemoryQue(entry);
				logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Pushed in OUTBOX queue");
				continue;


			}
			nCallChargingUrlFlag = 0;

			// Now i have to enter correct msg to queue for entry of smsoutbox					
			
			// message urlencode
			urlencode(dest, sms.msg);    
		
			nOperatorID = atoi(sms.srcAccount);			
			getOperatorInfoFromId(nOperatorID, operatorName);
			if (0 == strlen(operatorName) ||
				0 == strlen(sms.srcMN) ||
				0 == strlen(sms.dstMN))
			{
				logActivity(__FUNCTION__, src, E_ERROR, " Operator Name: %s", operatorName);
				cde.msgID = sms.msgID;
				insertIntoCDEFailedMemoryQue(cde);
				continue;
			}			
			
			// if operator configuration is wrong
			if (NULL == operatorConfigStruct[nOperatorID].operatorName || 
				0 == strlen(operatorConfigStruct[nOperatorID].operatorName)			)
			{
				logActivity(__FUNCTION__, src, E_ERROR, "SourceAccount configuration wrong :  %s", sms.msgID);
				cde.msgID = sms.msgID;
				insertIntoCDEFailedMemoryQue(cde);
				continue;
			}
			memset(url, MEMINIT_VALUE, sizeof(url));
			SPRINTF(url, sizeof(url), SMPP_CDE_URL, sms.srcMN, dest, sms.dstMN, "Y", operatorConfigStruct[nOperatorID].operatorName, sms.refID, sms.retryCount);
						
			// replace new line 
			char* urltmp = str_replace(url, "\r\n", "");
			logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " CDE URL: %s", url);
			
			
			chunk.size = 0; // no data at this point

			memset(chunk.buf, 0, sizeof(chunk.buf));

			curl = curl_easy_init();
			if (curl)
			{
				// Tell libcurl the URL
				curl_easy_setopt(curl, CURLOPT_URL, urltmp);				
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBufferCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);				
				res = curl_easy_perform(curl);

				if (CURLE_OK == res)
				{
					curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

					if (200 == http_code)
					{
						curl_easy_cleanup(curl);
						// Take final timestamp
						gettimeofday(&after, NULL);
						timeDiff = time_diff(before, after) / 1000;

						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "cdethread[%d] msgID %d  timediff: %f",cdethread, sms.msgID, timeDiff);

						//logActivity(__FUNCTION__, src, E_ERROR, "cdethread[%d] msgID %d", cdethread, sms.msgID);

						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "CDE RESPONSE: %s", chunk.buf);

						token = "";
						token = strtok(chunk.buf, "|");
						if (token != NULL)
						{
							strcpy(mainMsg, token);
							noreply = false;
							for (int i = 0; i < NOREPLY_STRING_NUMBER; i++)
							{
								if (strncmp(mainMsg, NOREPLY_STRINGS[i],
										strlen(NOREPLY_STRINGS[i])) == 0)
								{
									noreply = true;
									break;
								}
							}

							token = strtok(NULL, "|");

							// get service id
							SPRINTF(serviceId, sizeof(serviceId), "%s",	token == NULL ? "" : token);
																					
							// Now We need to get IsChargingEnable and ServiceConfigId							
							getServiceInfo(serviceId, &nCallChargingUrlFlag);						
							

							logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "MESSAGE:%s | SERVICE ID: %s", mainMsg, serviceId);
							// fill up the struct
							strcpy(entry.serviceID, serviceId);
							strcpy(entry.srcMN, sms.dstMN);
							strcpy(entry.dstMN, sms.srcMN);
							strcpy(entry.msg, mainMsg);
							strcpy(entry.msgType, "text"); // where it comes from
							
							entry.channel = sms.channel;
							if(NULL != sms.srcAccount)
							{
								strcpy(entry.destAccount, sms.srcAccount);
							}
							else
							{
								strcpy(entry.destAccount, "");
							}
														
							if(NULL != sms.refID && strlen(sms.refID) > 0)
							{
								strcpy(entry.refID, sms.refID);
							}
							else
							{								
								SPRINTF(entry.refID, sizeof(entry.refID), "%d",	sms.msgID);								
							}

							entry.isChargingEnabled = nCallChargingUrlFlag;							
							
							// If NOREPLY_STRINGS found, no reply sms will be inserted to smsoutbox
							if( true == noreply)
							{
								logActivity(__FUNCTION__, src, E_ERROR, "  MESSAGE: %s | MESSAGE ID: %d", entry.msg, sms.msgID);
							}
														
							if (false == noreply)
							{
								// Insert into smsQueue							
								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " INSERTED INTO SMS OUTBOX QUEUE | SOURCE: %s | DESTINATION: %s | MESSAGE: %s | MESSAGE ID: %d",						entry.srcMN, entry.dstMN, entry.msg, sms.msgID);
								
								insertIntoOutboxSingleMemoryQue(entry);
							}

							// insert into inbox response que
							insertIntoInboxResponseMemoryQue(sms);
						}
						else
						{
							logActivity(__FUNCTION__, src, E_ERROR, " CDE Response: No content, MESSAGE: %s, MESSAGE ID: %d, CDE url: %s", sms.msg, sms.msgID, urltmp);
							cde.msgID = sms.msgID;
							insertIntoCDEFailedMemoryQue(cde);
						}
					}
					else
					{						
						logActivity(__FUNCTION__, src, E_ERROR, " CALLING CDE URL FAILED , CDE URL: %s, HTTP_RESPONSE: %d", url, http_code);
						//for failed sms
						cde.msgID = sms.msgID;
						insertIntoCDEFailedMemoryQue(cde);
					}
				}
				else
				{
					logActivity(__FUNCTION__, src, E_ERROR, " CURL CALLING FAILED, MESSAGE: %s,  MESSAGE ID: %d, CURLcode: %d", sms.msg, sms.msgID, res);
					//for failed sms
					cde.msgID = sms.msgID;
					insertIntoCDEFailedMemoryQue(cde);
				}
			}
			else
			{
				logActivity(__FUNCTION__, src, E_ERROR, " curl_easy_init() failed , MESSAGE: %s,  MESSAGE ID: %d", sms.msg, sms.msgID);
				//for failed sms
				cde.msgID = sms.msgID;
				insertIntoCDEFailedMemoryQue(cde);
			}
		}
		Sleep(MIN_SLEEP);
	}

	return NULL;
}

void *UpdateSMSInbox(void *p)
{
	MYSQL *con = NULL;
	char query[MAXQUERYLENGTH], *src = "updatesnt";
	int ret;
	char strBatch[10000] = { 0 };
	int MAX_UPDATED_BATCH_SIZE = 2;
	int currentBatchSize = 0;
	struct smsinbox sms;
	struct cde_failed cde;
	int bDBConnected = 0;
	bool bSmsPending = false;
	int nret = 0;
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;

	bThreadStarted = true;

	while (1)
	{
		nret = 0;
		nret = getFromInboxResonseMemoryQueue(&sms);

		if (nret != -1)
		{
			if (0 == sms.msgID)
			{
				//ping message
				logActivity(__FUNCTION__, src, E_ERROR, "ping message, ret value: %d", nret);
				InboxResponseMemoryQuePingFlag = true;
				continue;
			}
			makeSMSBatch(sms.msgID, strBatch);
			currentBatchSize++;

			if (currentBatchSize == MAX_UPDATED_BATCH_SIZE)
			{
				memset(query, 0, sizeof(query));
				sprintf(query, "UPDATE smsinbox SET msgStatus = 'SENT' WHERE msgID IN (%s)", strBatch);


				while (1)
				{
					if (ConnectToDatabase(con, 0, src, "update smsinbox function"))
					{
						if (executeQuery(src, con, query))
						{
							logActivity(__FUNCTION__, src, E_ERROR, "Query: %s: connection: %p", query, con);
							memset(strBatch, 0, sizeof(strBatch));
							currentBatchSize = 0;

							closeConnection(src, con);
							break;
						}
					}
					else
					{
						Sleep(MIN_SLEEP);
					}
				}  //end while

			}
		}
		else
		{
			//nret = -1
			if (currentBatchSize > 0)
			{
				memset(query, 0, sizeof(query));
				sprintf(query, "UPDATE smsinbox SET msgStatus = 'SENT' WHERE msgID IN (%s)", strBatch);

				while (1)
				{
					if (ConnectToDatabase(con, 0, src, "update smsinbox function"))
					{
						if (executeQuery(src, con, query))
						{
							logActivity(__FUNCTION__, src, E_ERROR, "single batch Query: %s", query);
							memset(strBatch, 0, sizeof(strBatch));
							currentBatchSize = 0;

							closeConnection(src, con);
							break;
						}
					}
					else
					{
						Sleep(MIN_SLEEP);
					}
				}  //end while
			}
			else
			{
				Sleep(MIN_SLEEP);
			}

		}
		Sleep(MIN_SLEEP); //because ipcq insertion is slow
	}
	return NULL;
}

//void *UpdateSMSInbox(void *p) 
//{
//	MYSQL *con = NULL;
//	char query[MAXQUERYLENGTH], *src = "updatesnt";
//	int ret;	
//	char strBatch[10000] = {0};
//	int MAX_BATCH_SIZE = 50;
//	int currentBatchSize = 0;	
//	struct smsinbox sms;	
//	struct cde_failed cde;
//	int bDBConnected = 0;
//	bool bSmsPending = false;
//	int nret = 0;
//	struct timeval before, after;
//	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;
//
//	bThreadStarted = true;
//	
//	while (true)
//	{		
//		if (0 == bDBConnected)
//		{
//			bDBConnected = ConnectToDatabase(con, 0, src, "update smsinbox function");
//			if (1 == bDBConnected)
//			{
//				logActivity(__FUNCTION__, src, E_ERROR, "Connected to database");
//			}
//		}
//		if (1 == bDBConnected)
//		{				
//			while (true)
//			{					
//				nret = 0;
//				// If no sms pending, pick new sms from que
//				if (false == bSmsPending)
//				{
//					nret = getFromInboxResonseMemoryQueue(&sms);
//				}
//
//				logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "ret value: %d", nret);
//
//				if (-1 != nret)
//				{
//					logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "MessageID: %d",sms.msgID);
//
//					if (0 == sms.msgID)
//					{
//						InboxResponseMemoryQuePingFlag = true;
//						logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Found in INBOX RESPONSE queue");
//						continue;
//					}
//					makeSMSBatch(sms.msgID, strBatch);
//					currentBatchSize++;
//
//					if(currentBatchSize == MAX_BATCH_SIZE )
//					{
//
//						logActivity(__FUNCTION__, src, E_ERROR, "inside batch: %s", strBatch);
//						// Take initial timestamp
//						gettimeofday(&before, NULL);
//
//						SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'SENT' WHERE msgID IN (%s)", strBatch);
//
//						memset(strBatch, 0, sizeof(strBatch));
//						currentBatchSize = 0;
//						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " INBOX STATUS UPDATING AS SENT, QUERY: %s", query);
//						bSmsBeingSent = true;
//						ret = mysql_query(con, query);
//						bSmsBeingSent = false;
//						if (0 != ret)
//						{
//							logActivity(__FUNCTION__, src, E_ERROR, " Failed Query : %s, RESULT: %d", query, ret);
//							bSmsPending = true;
//
//							if (NULL != con)
//							{	
//								bDBConnected = 0;									
//								Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
//								if (ConnectToDatabase(con, 0, src, "Fetch row from smsoutbox") != 1)
//								{									
//									logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");
//									Sleep(1000);											
//									//continue;
//								}
//								break;
//							}										
//						}
//						else // update smsinbox is successful
//						{
//							// Take final timestamp
//							gettimeofday(&after, NULL);
//							timeDiff = time_diff(before, after) / 1000;
//
//							logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "update to SENT time diff: %f", timeDiff);
//							bSmsPending = false;
//						}						
//					}
//				}
//				if(currentBatchSize > 0)
//				{
//					logActivity(__FUNCTION__, src, E_ERROR, "my count: %d, strBatch: %s, maxbatch: %d", currentBatchSize, strBatch, MAX_BATCH_SIZE);
//					// Take initial timestamp
//					gettimeofday(&before, NULL);
//
//					SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'SENT' WHERE msgID IN (%s)", strBatch);
//
//					memset(strBatch, 0, sizeof(strBatch));
//					currentBatchSize = 0;
//					ret = 1;
//					ret = mysql_query(con, query);
//					if (0 != ret)
//					{						
//						logActivity(__FUNCTION__, src, E_ERROR, " Failed Query : %s, RESULT: %d", query, ret);
//						bSmsPending = true;
//
//						if (NULL != con)
//						{	
//							bDBConnected = 0;									
//							Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
//							if (ConnectToDatabase(con, 0, src, "Fetch row from smsoutbox") != 1)
//							{									
//								logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");
//								Sleep(1000);											
//								//continue;
//							}
//							break;
//						}										
//					}
//					else // update smsinbox is successful
//					{
//
//						// Take final timestamp
//						gettimeofday(&after, NULL);
//						timeDiff = time_diff(before, after) / 1000;
//						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "time difference: %f", timeDiff);
//						bSmsPending = false;
//					}
//				}
//				Sleep(MIN_SLEEP);
//			} //end while
//		}
//		else
//		{
//			logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");	
//			bDBConnected = 0;
//			Sleep(1000);
//		}
//		Sleep(MIN_SLEEP);		
//	}	
//	
//	return NULL;
//}

//void *UpdateFailedSMSInbox(void *p)
//{
//	MYSQL *con = NULL;
//	char query[MAXQUERYLENGTH], *src = "UpdtInbFail";
//	int ret;
//	char strBatch[10000] = {0};
//	int MAX_BATCH_SIZE = 2;
//	int currentBatchSize = 0;		
//	struct cde_failed cde;
//	int bDBConnected = 0;
//	bool bSmsPending = false;
//
//	bThreadStarted = true;
//	int nCdeRet = 0;
//	struct timeval before, after;
//	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;
//	
//	while (true)
//	{		
//		if (0 == bDBConnected)
//		{
//			bDBConnected = ConnectToDatabase(con, 0, src, "update smsinbox function");
//			if (1 == bDBConnected)
//			{
//				logActivity(__FUNCTION__, src, E_ERROR, "Connected to database");
//			}
//		}
//		if (1 == bDBConnected)
//		{				
//			while (true)
//			{
//				nCdeRet = 0;
//				if (false == bSmsPending)
//				{
//					nCdeRet = getFromCDEFailedMemoryQueue(&cde);
//				}					
//
//				if (-1 != nCdeRet)
//				{
//					logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "MessageID: %d", cde.msgID);
//					if (0 == cde.msgID) 
//					{
//						CDEFailedMemoryQuePingFlag = true;
//						logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS FOUND in CDE FAILED queue");
//						continue;
//					}
//					
//					makeSMSBatch(cde.msgID, strBatch);
//					currentBatchSize++;
//					if(MAX_BATCH_SIZE == currentBatchSize)
//					{
//
//						// Take initial timestamp
//						gettimeofday(&before, NULL);
//						SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'FAILED' WHERE msgID IN (%s)", strBatch);
//						memset(strBatch, 0, sizeof(strBatch));
//						currentBatchSize = 0;					
//						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " INBOX STATUS UPDATING AS FAILED, QUERY: %s", query);
//						
//						ret = 1;
//						bSmsBeingFailed = true;
//						ret = mysql_query(con, query);
//						bSmsBeingFailed = false;
//						if (0 != ret)
//						{
//							logActivity(__FUNCTION__, src, E_ERROR, " FAILED Query : %s, RESULT: %d", query, ret);
//							bSmsPending = true;
//
//							if (NULL != con)
//							{	
//								bDBConnected = 0;									
//								Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
//								if (ConnectToDatabase(con, 0, "main", "Fetch row from smsoutbox") != 1)
//								{									
//									logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");
//									Sleep(1000);										
//								}
//								break;
//							}										
//						}
//						else // update smsinbox is successful
//						{
//							// Take final timestamp
//							gettimeofday(&after, NULL);
//							timeDiff = time_diff(before, after) / 1000;
//
//
//							logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "update to FAILED time diff: %f", timeDiff);
//							bSmsPending = false;
//						}						
//					}						
//				}
//				if(currentBatchSize > 0)
//				{
//					// Take initial timestamp
//					gettimeofday(&before, NULL);
//					SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'FAILED' WHERE msgID IN (%s)", strBatch);
//					memset(strBatch, 0, sizeof(strBatch));
//					currentBatchSize = 0;
//					ret = 1;
//					ret = mysql_query(con, query);
//					if (0 != ret)
//					{
//						bSmsPending = true;
//						logActivity(__FUNCTION__, src, E_ERROR, " FAILED Query : %s, RESULT: %d", query, ret);
//						if (NULL!= con)
//						{	
//							bDBConnected = 0;
//							Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
//							if (ConnectToDatabase(con, 0, src, "Fetch row from smsoutbox") != 1)
//							{									
//								logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");
//								Sleep(1000);									
//							}
//							break;
//						}
//					}
//					else // update smsinbox is successful
//					{
//						// Take final timestamp
//						gettimeofday(&after, NULL);
//						timeDiff = time_diff(before, after) / 1000;
//
//
//						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "update to FAILED time diff: %f", timeDiff);
//						bSmsPending = false;
//					}
//				}
//				Sleep(MIN_SLEEP);
//			}
//		}
//		else 
//		{
//			logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");
//			bDBConnected = 0;
//			Sleep(1000);
//		}
//		Sleep(MIN_SLEEP);
//	}	
//	
//	return NULL;
//}
void *UpdateFailedSMSInbox(void *p)
{
	MYSQL *con = NULL;
	char query[MAXQUERYLENGTH], *src = "updFail";
	int ret;
	char strBatch[1000] = { 0 };
	int MAX_FAILED_UPDATED_BATCH_SIZE = 20;
	int currentBatchSize = 0;
	struct cde_failed cde;
	int bDBConnected = 0;
	bool bSmsPending = false;

	bThreadStarted = true;
	int nret;
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;

	while (1)
	{
		nret = 0;
		nret = getFromCDEFailedMemoryQueue(&cde);

		if (nret != -1)
		{
			if (0 == cde.msgID)
			{
				//ping message
				logActivity(__FUNCTION__, src, E_ERROR, "ping message, ret value: %d", nret);
				CDEFailedMemoryQuePingFlag = true;
				continue;
			}
			makeSMSBatch(cde.msgID, strBatch);
			currentBatchSize++;

			if (currentBatchSize == MAX_FAILED_UPDATED_BATCH_SIZE)
			{
				memset(query, 0, sizeof(query));
				sprintf(query, "UPDATE smsinbox SET msgStatus = 'FAILED' WHERE msgID IN (%s)", strBatch);


				while (1)
				{
					if (ConnectToDatabase(con, 0, src, "update smsinbox function"))
					{
						if (executeQuery(src, con, query))
						{
							logActivity(__FUNCTION__, src, E_ERROR, "Query: %s: connection: %p", query, con);
							memset(strBatch, 0, sizeof(strBatch));
							currentBatchSize = 0;

							closeConnection(src, con);
							break;
						}
					}
					else
					{
						Sleep(MIN_SLEEP);
					}
				}  //end while

			}
		}
		else
		{
			//nret = -1
			if (currentBatchSize > 0)
			{
				memset(query, 0, sizeof(query));
				sprintf(query, "UPDATE smsinbox SET msgStatus = 'FAILED' WHERE msgID IN (%s)", strBatch);

				while (1)
				{
					if (ConnectToDatabase(con, 0, src, "update smsinbox function"))
					{
						if (executeQuery(src, con, query))
						{
							logActivity(__FUNCTION__, src, E_ERROR, "single batch Query: %s", query);
							memset(strBatch, 0, sizeof(strBatch));
							currentBatchSize = 0;

							closeConnection(src, con);
							break;
						}
					}
					else
					{
						Sleep(MIN_SLEEP);
					}
				}  //end while
			}
			else
			{
				Sleep(MIN_SLEEP);
			}

		}
		Sleep(MIN_SLEEP); //because ipcq insertion is slow
	}

	return NULL;
}

void *batchInserterIntoSmsoutboxDB(void *p)
{
	MYSQL *con = NULL;
	char* src = "Insrt2Outbx";
	char query[MAXQUERYLENGTH + 500];
	int ret, nret;
	struct smsoutbox sms;
	int bDBConnected = 0;
	bool bSmsPending = false;
	char smsoutboxInsertionValuesQuery[MAXQUERYLENGTH * 2], queryRow[2000];
	int currentBatchSize;
	int lengthQueryInsertion;
	struct msqid_ds msqid_ds;
	int count, msgret;
	int pickNumber;

	int MAX_BATCH_SIZE = 50;

	bThreadStarted = true;

	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;

	while (true)
	{
		if (0 == bDBConnected)
		{
			bDBConnected = ConnectToDatabase(con, 0, src, "Insert into smsoutbox function");
			if (1 == bDBConnected)
			{
				logActivity(__FUNCTION__, src, E_ERROR, "Connected to database");
			}
		}

		if (1 == bDBConnected)
		{
			while (true)
			{
				if (!bSmsPending)
				{
					nret = 0;
					pickNumber = 0;
					currentBatchSize = 0;
					memset(smsoutboxInsertionValuesQuery, 0, sizeof(smsoutboxInsertionValuesQuery));
					memset(query, 0, sizeof(query));

					msgret = msgctl(SMS_OUTBOX_QUE_SHMID, IPC_STAT, &msqid_ds);



					if (-1 != msgret)
					{
						count = msqid_ds.msg_qnum;

						if (count >= MAX_BATCH_SIZE)
						{
							pickNumber = MAX_BATCH_SIZE;
						}
						else if (count < MAX_BATCH_SIZE)
						{
							pickNumber = count;
						}
						logActivity(__FUNCTION__, "count", E_DEBUG_MAJOR, "count : %d pickNumber : %d", count, pickNumber);
					}
				}


				while (0 < pickNumber && !bSmsPending)
				{
					nret = getFromOutboxMemoryQueue(&sms);

					logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "Message refID: %s",sms.refID);

					if (0 < strlen(sms.msgType)) 
					{
						if (0 == strcmp(sms.msgType, "PINGSMS"))
						{
							OutboxSingleMemoryQuePingFlag = true;
							logActivity(__FUNCTION__, "InHldrPng", E_DEBUG_MAJOR, "Ping SMS Found in OUTBOX queue");
							continue;
						}
					}

					memset(queryRow, 0, sizeof(queryRow));
					SPRINTF(queryRow, sizeof(queryRow), "('%s', '%s', \"%s\", '%s',%d, now(),now(), %d, '%s', '%s', '%s')", sms.srcMN, sms.dstMN, sms.msg, sms.refID, sms.channel, sms.isChargingEnabled, sms.destAccount, sms.destAccount, sms.serviceID);

					strcat(smsoutboxInsertionValuesQuery, queryRow);

					if (currentBatchSize < pickNumber - 1)
					{
						strcat(smsoutboxInsertionValuesQuery, ", ");
						logActivity(__FUNCTION__, "max", E_DEBUG_MAJOR, "current batchsize: %d", currentBatchSize);
						
					}
					else 
					{
						strcat(smsoutboxInsertionValuesQuery, "; ");
						logActivity(__FUNCTION__, "max", E_DEBUG_MAJOR, "max batchsize: %d", currentBatchSize);
						break;
					}
					currentBatchSize++;
				}


				if (0 < strlen(smsoutboxInsertionValuesQuery))
				{
					/*if (currentBatchSize < MAX_BATCH_SIZE)
					{
						lengthQueryInsertion = strlen(smsoutboxInsertionValuesQuery);

						strncpy(&smsoutboxInsertionValuesQuery[lengthQueryInsertion - 2], ";", 1);

						logActivity(__FUNCTION__, "max", E_DEBUG_MAJOR, "queue length :%s", smsoutboxInsertionValuesQuery);

						//smsoutboxInsertionValuesQuery[strlen(smsoutboxInsertionValuesQuery) - 2] = ';';
					}*/
					// Take initial timestamp
					gettimeofday(&before, NULL);

					SPRINTF(query, sizeof(query), "INSERT INTO smsoutbox (srcMN, dstMN, msg, refID, channel, writeTime,schedule, IsChargingEnable, srcAccount,destAccount, serviceID) VALUES %s", smsoutboxInsertionValuesQuery);

					logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " INSERING INTO OUTBOX, BATCH QUERY: %s", query);

					ret = 1;
					ret = mysql_query(con, query);

					if (0 != ret)
					{
						bSmsPending = true;
						logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY : %s RESULT : %d", query, ret);
						if (NULL != con)
						{
							Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
							if (ConnectToDatabase(con, 0, src, "Fetch row from smsoutbox") != 1)
							{
								bDBConnected = 0;
								logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");
								Sleep(1000);
							}
							break;
						}
					}
					else // instert in smsoutbox is successful
					{
						// Take final timestamp
						gettimeofday(&after, NULL);
						timeDiff = time_diff(before, after) / 1000;


						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "batch inserter time diff: %f", timeDiff);
						currentBatchSize = 0;						
						bSmsPending = false;
					}

				}
				Sleep(MIN_SLEEP);
			}//while (true)
		}
		else
		{
			logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");
			bDBConnected = 0;
			Sleep(1000);
		}
		Sleep(MIN_SLEEP);
	}

	return NULL;
}

void *InserterIntoDBsmsoutbox(void *p) 
{
	MYSQL *con = NULL;
	char* src = "Insrt2Outbox";	
	char query[MAXQUERYLENGTH];
	int ret;
	struct smsoutbox sms;
	int bDBConnected = 0;
	bool bSmsPending = false;

	bThreadStarted = true;
	int nret = 0;
	
	while (true)
	{
		if (0 == bDBConnected)
		{
			bDBConnected = ConnectToDatabase(con, 0, src, "Insert into smsoutbox function");
			if (1 == bDBConnected)
			{
				logActivity(__FUNCTION__, src, E_ERROR, "Connected to database");
			}
		}

		if (1 == bDBConnected)
		{				
			while (true)
			{			
				nret = 0;
				if (false == bSmsPending)
				{
					nret = getFromOutboxMemoryQueue(&sms);
				}
				
				if (-1 != nret)
				{							
					SPRINTF(query, sizeof(query),
						"INSERT INTO smsoutbox (srcMN, dstMN, msg, refID, channel, writeTime,schedule, IsChargingEnable, srcAccount,destAccount, serviceID) VALUES ('%s', '%s', \"%s\", '%s',%d, now(),now(), %d, '%s', '%s', '%s')",
						sms.srcMN, sms.dstMN, sms.msg, sms.refID, sms.channel, sms.isChargingEnabled, sms.destAccount, sms.destAccount,sms.serviceID);

					logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " INSERING INTO OUTBOX, QUERY: %s", query);
					ret = 1;
					ret = mysql_query(con, query);
					if (0 != ret)
					{
						bSmsPending = true;
						logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY : %s RESULT : %d", query, ret);					
						if (NULL != con) 
						{	
							Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
							if (ConnectToDatabase(con, 0, src, "Fetch row from smsoutbox") != 1)
							{
								bDBConnected = 0;
								logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");
								Sleep(1000);								
							}
							break;
						}
					}
					else // instert in smsoutbox is successful
					{
						bSmsPending = false;
					}
				}
				Sleep(MIN_SLEEP);
			}
		}
		else
		{
			logActivity(__FUNCTION__, src, E_ERROR, " Couldn't connect to database");		
			bDBConnected = 0;
			Sleep(1000);
		}
		Sleep(MIN_SLEEP);
	}

	return NULL;
}

void* ReinitializeSMS(void *p) 
{
	MYSQL *con = NULL;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char* src = "InHldrPng";

	char query[MAXQUERYLENGTH];
	int count, maxMsgID;
	int ret = 0;
	struct smsoutbox sms;
	struct smsinbox entry;
	int bDBConnected = 0;
	bThreadStarted = true;
	int affected_rows = 0;
	char pingSentTime[20];
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;

	while (true)
	{
		if (0 == bDBConnected)
		{
			bDBConnected = ConnectToDatabase(con, 0, src, "Reinitialize SMS");
			if (1 == bDBConnected)
			{
				logActivity(__FUNCTION__, src, E_ERROR, " Connected to database");
			}
		}
		//////////

		if (1 == bDBConnected && NULL != con)
		{
			while (true)//false == isPingSent)
			{
				logActivity(__FUNCTION__, src, E_ERROR, "---Sending Ping Ball in CDE Handler---");

				//SPRINTF(query, sizeof(query), "SELECT COUNT(*), MAX(msgID),(NOW() - INTERVAL %d MINUTE) FROM smsinbox WHERE msgStatus = 'PROCESSING' and rcvTime < (NOW() - INTERVAL %d MINUTE) AND retryCount <= %d ", PING_BALL_DELAY, PING_BALL_DELAY, RETRYCOUNT);

				SPRINTF(query, sizeof(query), "SELECT COUNT(*), MAX(msgID),(NOW() - INTERVAL %d MINUTE) FROM smsinbox WHERE msgStatus = 'PROCESSING' AND retryCount <= %d ", PING_BALL_DELAY, RETRYCOUNT);

				logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "QUERY : %s", query);

				//execute the query
				ret = 1; // reinit as an error
				ret = mysql_query(con, query);

				// Query failed
				if (0 != ret)
				{
					logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY : %s Result: %d", query, ret);
					if (NULL != con)
					{
						Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
						con = NULL;
						bDBConnected = 0;
						Sleep(1000);
						break;
					}
				}

				// Get the result
				result = mysql_store_result(con);

				// reinitialize that is update as que
				if ((NULL != result) && (row = mysql_fetch_row(result)) != false)
				{
					count = atoi(row[0]);

					if (count > 0)
					{

						//there are some rows in processing state
						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "Messages are in processing state");
						if (NULL != result)
						{
							mysql_free_result(result);
							result = NULL;
						}
						maxMsgID = atoi(row[1]);

						if (row[2]) strcpy(pingSentTime, row[2]);
						else {
							// Form the current timestamp like MySQL datetime format and then assign
							strcpy(pingSentTime, "2015-09-16 04:17:06");
						}
						// some rows in processing state
						isPingSent = false;

						//create ping sms
						entry.msgID = 0;

						OutboxSingleMemoryQuePingFlag = false;
						CDEFailedMemoryQuePingFlag = false;
						InboxResponseMemoryQuePingFlag = false;

						//push ping SMS in inbox single queue
						insertIntoInboxSingleMemoryQue(entry);

						logActivity(__FUNCTION__, "QUE", E_DEBUG_MAJOR, " Ping SMS inserted into inbox single queue Message ID: %d", entry.msgID);

						isPingSent = true;

						//logActivity(__FUNCTION__, src, E_ERROR, "---isPingSent = false---");
						while (true)
						{
							if (OutboxSingleMemoryQuePingFlag && CDEFailedMemoryQuePingFlag && InboxResponseMemoryQuePingFlag && !bSmsBeingSent && !bSmsBeingFailed && !bSmsBeingQueInRetryFailed)
							{

								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "IPC Ques are empty");
								// Take initial timestamp
								gettimeofday(&before, NULL);

								// making msgStatus FAILED when the retrycount is eaual to RETRYCOUNT (configurable)
								SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'FAILED'  WHERE msgStatus = 'PROCESSING' and rcvTime < '%s' AND retryCount >= %d AND msgID <= %d", pingSentTime, RETRYCOUNT, maxMsgID);

								//SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'FAILED'  WHERE msgStatus = 'PROCESSING' and rcvTime < '%s' AND retryCount >= %d AND msgID <= %d",pingSentTime, RETRYCOUNT, maxMsgID);

								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " Reinitialize STATUS AS FAILED, QUERY: %s", query);

								ret = 1;
								ret = mysql_query(con, query);

								if (0 != ret)
								{
									logActivity(__FUNCTION__, src, E_ERROR, " Reinitialize FAILED, QUERY : %s Result: %d", query, ret);
									if (NULL != con)
									{
										Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
										bDBConnected = 0;
										con = NULL;
									}

									break;
								}
								// Take final timestamp
								gettimeofday(&after, NULL);
								timeDiff = time_diff(before, after) / 1000;
							
								affected_rows = mysql_affected_rows(con);

								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "timediff %f Affected Rows for Making PROCESSING->FAILED, %ld",timeDiff, affected_rows);
								
								logActivity(__FUNCTION__, src, E_ERROR, "Affected Rows for Making PROCESSING->FAILED, %ld",affected_rows);

								//End making FAILED

								// Take initial timestamp
								gettimeofday(&before, NULL);

								SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'QUE', retryCount = retryCount + 1 where msgStatus = 'PROCESSING' and rcvTime < '%s' AND retryCount < %d AND msgID <= %d ", pingSentTime, RETRYCOUNT, maxMsgID);
								
								//SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'QUE', retryCount = retryCount + 1 where msgStatus = 'PROCESSING' and rcvTime < '%s' AND retryCount < %d AND msgID <= %d ",pingSentTime, RETRYCOUNT, maxMsgID);
								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " Reinitialize STATUS AS QUE, QUERY: %s", query);

								ret = 1;
								ret = mysql_query(con, query);

								if (0 != ret)
								{
									if (NULL != result)
									{
										mysql_free_result(result);
										result = NULL;
									}

									logActivity(__FUNCTION__, src, E_ERROR, " Reinitialize FAILED, QUERY : %s Result: %d", query, ret);
									if (NULL != con)
									{
										Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
										bDBConnected = 0;
										con = NULL;
									}

									break;
								}
								// Take final timestamp
								gettimeofday(&after, NULL);
								timeDiff = time_diff(before, after) / 1000;

								affected_rows = mysql_affected_rows(con);

								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "timediff %f Affected Rows for Making PROCESSING->QUE, %ld",timeDiff, affected_rows);

								logActivity(__FUNCTION__, src, E_ERROR, "Affected Rows for Making PROCESSING->QUE, %ld", affected_rows);

								//end making QUE
								break;
							}
							Sleep(100);
						}

					}
					else
					{
						logActivity(__FUNCTION__, src, E_ERROR, "No messages in processing state.---isPingSent = true---");
						isPingSent = true;

						// Free the reult
						mysql_free_result(result);
						result = NULL;

						Sleep(MIN_SLEEP * 100); //added
					}

					if (NULL != result)
					{
						// Free the reult
						mysql_free_result(result);
						result = NULL;
					}
				}
				else
				{
					logActivity(__FUNCTION__, src, E_ERROR, " Unexpected result from DB while re-initializing sms");
				}
				//Sleep(MIN_SLEEP);

				logActivity(__FUNCTION__, src, E_ERROR, "Going to pingball delay.---isPingSent = true---");

				// Delay for an interval
				Sleep(PING_BALL_DELAY * 60 * 1000);


				if (0 == bDBConnected)
				{
					con = NULL;
					break;
				}
				//Sleep(MIN_SLEEP);
			}
		}
		else
		{
			logActivity(__FUNCTION__, src, E_ERROR, " DB CONNECTION FAILED");
			bDBConnected = 0;
			Sleep(1000);
		}
		Sleep(MIN_SLEEP);
	}

	return NULL;
}

void Handler(char *src, void *ptr, char *buf, int buflen, Event e) {

	char buffer[20], reply[20];
	int len, ret;	
	bool serviceEmpty = false;
	int i = 0, j = 0;

	printf("Command found: %s, length: %d\n", buf, strlen(buf));
		
	len = strlen(buf);
	for(i = 0; i < len; i++){
		//printf("found char: %c-%d\n", *buf, (int)*buf);
		if (buf[i] != '\n' && buf[i] != '\r') 
		{			
			buffer[j++] = buf[i];			
		}
	}
	
	buffer[j] = '\0';

	NetworkThread *NT = (NetworkThread *)ptr;

	printf("buffer formed: %s, length: %d\n", buffer, strlen(buffer));
		
	if (strcmp(buffer,"CDE") == 0) 
	{	
		SPRINTF(reply, sizeof(reply), "reloaded\n");
		ret = send(NT->s, reply, sizeof(reply), 0);
		logActivity(__FUNCTION__, "InbxHndlr", E_ERROR, " COMMAND_SERVER  Reload response: %s", reply);  
		
		// flag is off as emptying application queues is starting and 
		// PickQUEDSMSFromInbox is restricted from enqueuing application queue
		bReloadComplete = false;

		while (true)
		{
			for (i = 0; i < TOTAL_SHORTCODE_COUNT; i++)
			{
				if (false == emptyInboxQueue(&shortcodeQueueArray[i]))
				{
					serviceEmpty = false;
					Sleep(MIN_SLEEP);
					break;
				}
				else
				{
					serviceEmpty = true;
				}
			}
			if (true == serviceEmpty)
				break;

			Sleep(MIN_SLEEP);
		}

		// flag for reload shortcode config
		bReloadingShortcode = true;
		
		//flag is on as emptying application queues is completed
		bReloadComplete = true;		
		logActivity(__FUNCTION__, "InbxHndlr", E_DEBUG_MAJOR, "bReloadingShortcode : %d and bReloadComplete :%d",bReloadingShortcode ,bReloadComplete);

		// Queue deletion
		for (j = 0; j <TOTAL_SHORTCODE_COUNT; j++)	deleteInboxQueue(shortcodeQueueArray[j]) ;
		
		// Initialize 2D array
		for(i = 0; i < MAX_SHORTCODE_COUNT; i++) shortcode_wise2d_queStruct[i].exists = false;
			

		loadShortcode();
		
		logActivity(__FUNCTION__, "InbxHndlr", E_DEBUG_MAJOR, " Loading Short Code by command Total shortcode : %d", TOTAL_SHORTCODE_COUNT);
		
		// Queue creation
		for (i = 0; i < TOTAL_SHORTCODE_COUNT; i++) shortcodeQueueArray[i] = createInboxQueue();		

		bReloadingShortcode = false;
	}
	else 
	{
		SPRINTF(reply, sizeof(reply), "failed\n");
		ret = send(NT->s, reply, sizeof(reply), 0);
		logActivity(__FUNCTION__, "InbxHndlr", E_ERROR, " COMMAND_SERVER  Reload response: %s", reply);  
	}	
}

void Controller(char *src, NetworkThread *NT, void *p) {

	NT->RxStatus=THREAD_RUNNING;
	while(NT->RxStatus==THREAD_RUNNING)
		Sleep(MIN_SLEEP);
}

void *runCommandServer(void *p) 
{
	bThreadStarted = true;
	logActivity(__FUNCTION__, "InbxHndlr", E_DEBUG_MAJOR, " COMMAND_PROCESS_PORT : %d", COMMAND_PROCESS_PORT);	
	char *thName = "CmdServer";
	initTCPServer(thName, Controller, Handler, COMMAND_PROCESS_PORT);  //its a great problem

	return NULL;
}

int loadOperatorInfo()
{
	/*
	Function: loadOperatorInfo

	Reads all operator related information and store them in local structure

	Parameters:

	fileName - Name of the configuration file.

	Returns:

	1 for success and 0 for failure.
	 */

	char operatorInfoUrl[300], buffer[100000];

	// Hit the operator info loading URL
	SPRINTF(operatorInfoUrl, sizeof(operatorInfoUrl), OPERATORINFO_URL);
	if (0 == strlen(operatorInfoUrl))
	{
		return -1;
	}
	CallURL(operatorInfoUrl, buffer);
	
	if (0 == strlen(buffer))
	{
		return -1;
	}
	char *row, *item, *colSeperator = "|", *rowSeperator = "\r\n", *nextToken = NULL, *nextRow = NULL;
	int rowcounter = 0, id = 0;

	
	row = STRTOK(buffer, rowSeperator, &nextRow);

	while (row != NULL){
		
		item = STRTOK(row, colSeperator, &nextToken);
		
		if(NULL != item)
		{				
			id = atoi(item);
			operator_id[rowcounter] = id;
		}

		item = STRTOK(NULL, colSeperator, &nextToken);

		if(NULL != item)
		{			
			strcpy(operatorConfigStruct[id].operatorName, item);				
		}

		item = STRTOK(NULL, colSeperator, &nextToken);
		if(NULL != item)
		{			
			strcpy(operatorConfigStruct[id].MT_URL, item);			
		}
				
		row = STRTOK(NULL, rowSeperator, &nextRow);	
		rowcounter++;
		
	}
	TOTAL_OPERATOR_COUNT = rowcounter;
	return 0;
}

void getOperatorInfoFromId(int nOperatorId, char* sOperatorName)
{	
	if (nOperatorId < MAX_OPERATOR_COUNT)
	{
		strcpy(sOperatorName, operatorConfigStruct[nOperatorId].operatorName);		
	}
	else
	{
		strcpy(sOperatorName, "");		
	}	
}

void *RetryForFailedSMS(void *p)
{
	MYSQL *con = NULL;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[MAXQUERYLENGTH], *src = "RetryHandler";
	char strBatch[10000] = { 0 };
	int MAX_BATCH_SIZE = 1, currentBatchSize = 0;	
	int ret;
	bThreadStarted = true;

	int bDBConnected = 0;
	bool bSmsPending = false;
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;

	while (true)
	{
		if (0 == bDBConnected)
		{
			bDBConnected = ConnectToDatabase(con, 0, src, "Retry failed sms handler function");
		}

		if (1 == bDBConnected)
		{			
			logActivity(__FUNCTION__, src, E_ERROR, "Connected to database");

			while (true)
			{
				// Take initial timestamp
				gettimeofday(&before, NULL);
				// first select the failed sms				
				// if retryCount > RETRYCOUNT , don't try
				// if retry gap less than RETRY_DELAY , don't try				
				 
				// Pick a constant number (say 1000, that will be put as LIMIT value in SQL query) of SMS with state = QUE for i-th service	from DB
				SPRINTF(query, sizeof(query), "UPDATE smsinbox SET msgStatus = 'QUE', retryCount = retryCount+1 "
					"WHERE msgStatus = 'FAILED' AND retryCount < %d  and (NOW() - rcvtime)> %d ", RETRYCOUNT, RETRY_DELAY);	
				
				// If query execution failed, write log. If connection is lost, attempt to reconnect
				//logActivity(__FUNCTION__, src, E_ERROR, "FAILED to QUE Query: %s", query);
				ret = 1;
				bSmsBeingQueInRetryFailed = true;
				ret = mysql_query(con, query);
				bSmsBeingQueInRetryFailed = false;
				if (0 != ret)
				{
					bDBConnected = 0; 					
					logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY: %s, RESULT : %d", query, ret);					
					if (NULL != con)
					{
						Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
						Sleep(1000);						
						break;
					}
				}
				// Take final timestamp
				gettimeofday(&after, NULL);
				timeDiff = time_diff(before, after) / 1000;


				logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "time diff: %f", timeDiff);

				Sleep(RETRY_DELAY*1000);
			}		
		}
		else
		{
			logActivity(__FUNCTION__, src, E_ERROR, " Retry Failed SMS thread couldn't connect to db");
			bDBConnected = 0;
			Sleep(1000);
		}
		Sleep(MIN_SLEEP);
	}
	return NULL;
}

void *RetryForCPUrl(void *p)
{
	MYSQL *con = NULL;
	MYSQL_ROW row;
	MYSQL_RES *result = NULL;
	char query[MAXQUERYLENGTH], *src = "CpUrlHndlr";
	char sCPUrl[1000];
	int id;
	char *url;

	long http_code = 0;
	CURL *curl, *curl_handle;
	CURLcode res;
	int ret;
	struct cdebuffer chunk;
	struct timeval before, after;
	double timeDiff = 0.0, thresholdTimeDiff = 5000.0;

	bThreadStarted = true;
	int bDBConnected = 0;

	while (true)
	{
		if (0 == bDBConnected)
		{
			bDBConnected = ConnectToDatabase(con, 0, src, "Retry CP URL handler function");
		}

		if (1 == bDBConnected)
		{			
			logActivity(__FUNCTION__, src, E_ERROR, "Connected to database");

			while (true)
			{
				SPRINTF(query, sizeof(query), "SELECT id, cpUrl FROM cp_url_log WHERE urlType = 1 AND retryCount < %d AND statusCode = ''", RETRYCOUNT);	

				// If query execution failed, write log. If connection is lost, attempt to reconnect
				ret = 1;
				ret = mysql_query(con, query);
				if (0 != ret)
				{
					bDBConnected = 0; 					
					logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY: %s, RESULT : %d", query, ret);					
					if (NULL != con)
					{
						Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);
						con = NULL;
						Sleep(1000);						
						break;
					}
				}
				else 
				{				
					result = mysql_store_result(con);
					//int numfields = mysql_num_fields(result);
					//MYSQL_FIELD *mfield;

					while( NULL != result && (row = mysql_fetch_row(result)))
					{
						// Take initial timestamp
						gettimeofday(&before, NULL);

						id = atoi(row[0]);
						memset(sCPUrl, MEMINIT_VALUE, sizeof(sCPUrl));
						
						// urlencode
						//urlencode(sCPUrl, row[1]); 
						strcpy(sCPUrl, row[1]);

						logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "CP URL: %s", sCPUrl);
						
						chunk.size = 0; // no data at this point

						memset(chunk.buf, 0, sizeof(chunk.buf));

						curl = curl_easy_init();
						if (curl)
						{
							// Tell libcurl the URL
							curl_easy_setopt(curl, CURLOPT_URL, sCPUrl);				
							curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBufferCallback);
							curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);				
							res = curl_easy_perform(curl);

							http_code = 0;

							if (CURLE_OK == res)
							{
								curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

								if (0 == http_code)
								{
									curl_easy_cleanup(curl);
									// Take final timestamp
									gettimeofday(&after, NULL);
									timeDiff = time_diff(before, after) / 1000;

									logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, "timediff %f CP URL: %s, CURLINFO_RESPONSE_CODE: %d", timeDiff, sCPUrl, http_code);
									
									logActivity(__FUNCTION__, src, E_ERROR, "CP URL: %s, CURLINFO_RESPONSE_CODE: %d",sCPUrl, http_code);	

									SPRINTF(query, sizeof(query), "UPDATE cp_url_log SET retryCount = retryCount +1 WHERE id = %d",  id); 
								}							
								else
								{
									SPRINTF(query, sizeof(query), "UPDATE cp_url_log SET statusCode = %d, retryCount = retryCount +1 WHERE id = %d",  http_code, id); 
								}						
							}
							else
							{
								logActivity(__FUNCTION__, src, E_ERROR, " CP URL: %s, CURLcode: %d", sCPUrl, res);	
								SPRINTF(query, sizeof(query), "UPDATE cp_url_log SET retryCount = retryCount +1 WHERE id = %d",  id); 
							}

							ret = 1;
							ret = mysql_query(con, query);
							// If query execution failed, write log. If connection is lost, attempt to reconnect
							if (0 != ret)
							{					
								logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY: %s, RESULT : %d", query, ret);
								if (NULL != con)
								{
									bDBConnected = 0;

									// Free the result
									if (NULL != result)
									{
										mysql_free_result(result);
										result = NULL;
									}

									Finish_with_error(con, __FILE__, __FUNCTION__, __LINE__);	
									con = NULL;
									Sleep(1000);											
									break;
								}
							}
							else
							{
								logActivity(__FUNCTION__, src, E_DEBUG_MAJOR, " CP URL %s, id %id QUERY: %s, RESULT : %d", sCPUrl, id,query, ret);
							}
						}
					}
					
					// Free the result
					if (NULL != result)
					{
						mysql_free_result(result);
						result = NULL;
					}
				}

				if (0 == bDBConnected)
				{
					con = NULL;
					break;
				}
				Sleep(MIN_SLEEP);
			}
		}	
		else
		{
			logActivity(__FUNCTION__, src, E_ERROR, " Retry for CP URL thread couldn't connect to db");
			bDBConnected = 0;
			Sleep(1000);
		}
		Sleep(MIN_SLEEP);
	}
	return NULL;
}

#endif //_FUNCTION_H_
