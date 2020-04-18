#ifndef _COMMON_H_
#define _COMMON_H_


/*Initialize thread parameter*/
void initThreadAttr(pthread_attr_t *attr)
{
	int threadReturnValue = SUCCESS;
	int PTHREAD_STACK_SIZE_NETWORK = 1024 * PTHREAD_STACK_SIZE_NETWORK_KB;  /*stack size for network thread (bChRx)*/
	int PTHREAD_STACK_SIZE = 1024 * PTHREAD_STACK_SIZE_KB;				 // stack size bChHandler and other threads.

#ifdef COMPILER_GCC
	threadReturnValue = pthread_attr_init(attr);
	if (threadReturnValue != SUCCESS)
	{
		logActivity(__FUNCTION__, "main", E_ERROR, "pthread_attr_init failed with error code %d.", threadReturnValue);
		exit(0);
	}

	threadReturnValue = pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
	if (threadReturnValue != SUCCESS)
	{
		logActivity(__FUNCTION__, "main", E_ERROR, "pthread_attr_setdetachstate failed with error code %d.", threadReturnValue);
		exit(0);
	}
	threadReturnValue = pthread_attr_setstacksize(attr, PTHREAD_STACK_SIZE);
	if (threadReturnValue != SUCCESS)
	{
		logActivity(__FUNCTION__, "main", E_ERROR, "pthread_attr_setstacksize failed with error code %d.", threadReturnValue);
		exit(0);
	}
#endif

	// Finally initialize socket framework thread attributes.
	threadReturnValue = initThreadAttributes(PTHREAD_STACK_SIZE_NETWORK);
	if (threadReturnValue != SUCCESS)
	{
		logActivity(__FUNCTION__, "main", E_ERROR, "Thread initialization failed for socket framework with error code %d.", threadReturnValue);
		exit(0);
	}
}

char *str_replace(const char *string, const char *substr, const char *replacement){

	char *tok = NULL, *newstr = NULL, *oldstr = NULL, *head = NULL;

	/* Return source string if either substr or replacement is NULL */
	if (substr == NULL || replacement == NULL) return strdup(string);
	newstr = strdup(string);
	head = newstr;

	while ((tok = strstr(head, substr))) {

		oldstr = newstr;
		newstr = (char*)malloc(strlen(oldstr) - strlen(substr) + strlen(replacement) + 1);

		/* Free old string and return NULL if failed to alloc memory */
		if (newstr == NULL){
			free(oldstr);
			return NULL;
		}
		memcpy(newstr, oldstr, tok - oldstr);
		memcpy(newstr + (tok - oldstr), replacement, strlen(replacement));
		memcpy(newstr + (tok - oldstr) + strlen(replacement), tok + strlen(substr), strlen(oldstr) - strlen(substr) - (tok - oldstr));
		memset(newstr + strlen(oldstr) - strlen(substr) + strlen(replacement), 0, 1);

		/* Move back head right after the last replacement */
		head = newstr + (tok - oldstr) + strlen(replacement);
		free(oldstr);
	}

	return newstr;
}

char *ltrim(char *s) {

	while (isspace(*s)) s++;
	return s;
}

/* Make url to send using the url in operators
*prefix - The caller
*url - Url from database that is to be parsed and modified
*source - Source mobile number
*dest - Destination mobile number
*message - Message to send
*serviceid - Service id associated with the message
*/
void setUrlParamters(char *prefix, char *url, char *source, char *dest, char *message, char *serviceid = NULL, char *op = NULL, char *mid = NULL, char *smsclientid = NULL )
{
	char modifiedUrl[2000];
	int i = 0, k = 0;

	memset(modifiedUrl, MEMINIT_VALUE, sizeof(modifiedUrl));
	strcpy(modifiedUrl, "");
	
	while (url[i] != '\0')
	{
		if (url[i] == '%')
		{
			if (strncmp(&url[i], "%sc", 3) == 0 && source != NULL)
			{
				strcat(modifiedUrl, source);
				k = strlen(modifiedUrl);
				i = i + 3;

			}
			else if (strncmp(&url[i], "%mn", 3) == 0 && dest != NULL)
			{
				strcat(modifiedUrl, dest);
				k = strlen(modifiedUrl);
				i = i + 3;

			}
			else if (strncmp(&url[i], "%sd", 3) == 0 && serviceid != NULL)
			{
				strcat(modifiedUrl, serviceid);
				k = strlen(modifiedUrl);
				i = i + 3;
			}
			else if (strncmp(&url[i], "%msg", 4) == 0 && message !=NULL)
			{
				strcat(modifiedUrl, message);
				k = strlen(modifiedUrl);
				i = i + 4;
			}
			else if (strncmp(&url[i], "%scid", 5) == 0 && smsclientid != NULL)
			{
				strcat(modifiedUrl, smsclientid);
				k = strlen(modifiedUrl);
				i = i + 5;
			}
			else if (strncmp(&url[i], "%op", 3) == 0  && op != NULL)
			{
				strcat(modifiedUrl, op);
				k = strlen(modifiedUrl);
				i = i + 3;
			}

			else if (strncmp(&url[i], "%msid", 5) == 0 && mid != NULL)
			{
				strcat(modifiedUrl, mid);
				k = strlen(modifiedUrl);
				i = i + 5;
			}
      else {
          strcat(modifiedUrl, "%"); 
          k = strlen(modifiedUrl);
          i++;
      }
    }
		else
		{
			k = strlen(modifiedUrl);
			modifiedUrl[k] = url[i];
			i++;

		}
	}
	modifiedUrl[k + 1] = 0;
	memset(url, MEMINIT_VALUE, sizeof(url));
	strcpy(url, modifiedUrl);
}

/*
To show database connectivity error.
*/
void Finish_with_error(MYSQL *con,char *src) {

	char log[1500];

	if (con) {
		fprintf(stderr, "%s\n", mysql_error(con));
		SPRINTF(log, sizeof(log), "%s", mysql_error(con));
		logActivity(__FUNCTION__, "inbox_sql", E_ERROR, "%s::%s",src, log);
		if(con) mysql_close(con);
	}
}

/*
To show database connectivity error.
*/
void Finish_with_error(MYSQL *con, const char* srcFile, const char* srcFunction, int line) 
{
	char log[1500];

	if (con) {
		fprintf(stderr, "%s\n", mysql_error(con));
		SPRINTF(log, sizeof(log), "%s", mysql_error(con));
		logActivity(srcFunction, "inbox_sql", E_ERROR, "File: %s, LINE:%d, LOG:%s", srcFile, line, log);
		if (con)
		{
			mysql_close(con);
			con = NULL;
		}
	}
}

//new code
void closeConnection(char *src, MYSQL *&con)
{
	mysql_close(con);
	con = NULL;
}

int executeQuery(char *src, MYSQL *con, char *query)
{
	if (mysql_query(con, query))
	{
		logActivity(__FUNCTION__, src, E_ERROR, " FAILED QUERY: %s, RESULT : %d , error no: %d", query, mysql_query(con, query), mysql_errno(con));
		return 0;
	}
	return 1;
}

/*
creates the connection to database
*/
int ConnectToDatabase(MYSQL *&mh, int thid, char *srcName, char *successMsg) {

	char log[1500];
	mh = mysql_init(NULL);
	if (mysql_real_connect(mh, SERVER_NAME, DB_USER, DB_USERPASS, DB_NAME, 0, NULL, 0) == NULL) {
		logActivity(__FUNCTION__, srcName, E_ERROR, "MySQL Error: %s", mysql_error(mh));
		return -1;
	}
	else {
		return 1;
	}
}

/*Write to any table
*prefix - The function that is calling this function
*tableName - Table to write
*src - Source mobile number
*dest - Destination mobile number
*msg - Message to write to table
*serviceID - Service id associated with the message
**/
void WriteToTable(char *prefix, char tableName[15], char src[20], char dest[20], char msg[3000], char *serviceID = NULL, char *channel = NULL, char *refid = NULL, char *destAccount = NULL) //here parameters will be tablename, src, dest, msg
{
	char qry[500], log[1500];

	// Adding slash to single quote, otherwise query won't be properly formed
	strcpy(msg, str_replace(msg, "'", "\\'"));

	if (strcmp(tableName, "smsinbox") == 0)
		SPRINTF(qry, sizeof(qry), "INSERT INTO %s(srcMN, dstMN, msg, rcvTime, msgStatus, retryCount, srcAccount) VALUES ('%s', '%s', '%s', NOW(), 'QUE', 0, 'BL')",
		tableName, src, dest, msg);

	else if (strcmp(tableName, "smsoutbox") == 0) {

		if (strcmp(channel, "SMPP") == 0 || channel == NULL) 
			SPRINTF(qry, sizeof(qry), "INSERT INTO %s(srcMN, dstMN, msg, writeTime, sentTime, msgStatus, retryCount, srcTON, srcNPI, MsgType, Esm_Class, Data_Coding, smsPart, Totalpart, refID, schedule, ServiceID) VALUES ('%s', '%s', '%s', now(),now(),'QUE',0,%d,%d,'TEXT',0,0,1,1,'%s', now(), '%s')", tableName, src, dest, msg, SRCTON, SRCNPI, refid, serviceID);
		else 
			SPRINTF(qry, sizeof(qry), "INSERT INTO %s(srcMN, dstMN, msg, writeTime, sentTime, msgStatus, retryCount, srcTON, srcNPI, MsgType, Esm_Class, Data_Coding, smsPart, Totalpart, refID, schedule, ServiceID, channel, destAccount) values ('%s', '%s', '%s', now(),now(),'QUE',0,%d,%d,'TEXT',0,0,1,1,'%s', now(), '%s', '%s', '%s')", tableName, src, dest, msg, SRCTON, SRCNPI, refid, serviceID, channel, destAccount);
	}

	logActivity(__FUNCTION__, "main", E_INFO, "Execution database query: %s", qry);

	MYSQL *con = NULL;

	ConnectToDatabase(con, 0, prefix, "Writing into table");

	if (mysql_query(con, qry)) Finish_with_error(con,"WriteToTable");

	mysql_close(con);

}

/* Testing for function calling for main function  */
/********************* For connecting with other IP**********************/

void InitializeConnection(SOCKET *s, char *src, char *ip, int port) {

	int ret;
	char log[1500];

	ret = InitSocket(s, src, 0);
	if (ret != SUCCESS) logActivity(__FUNCTION__, src, E_ERROR, "Error Initializing Socket at line %d of %s", __LINE__, __FILE__);

	ret = ConnectPeer(s, ip, port, src);
	if (ret != SUCCESS) logActivity(__FUNCTION__, src, E_ERROR, "Error in ConnectPeer!!! SMSC IP: %s:%d on socket %d at line %d of %s", SMSC_IP, port, s, __LINE__, __FILE__);	
}


/*------------------For CDE php caller------------------*/
/*Convert all characters to upper case*/

void ConvertToUpper(char *str)
{
	int len = strlen(str), i = 0;

	while (str[i] != '\0')
	{
		if (str[i] >= 'a' && str[i] <= 'z') str[i] = 'A' + str[i] - 'a';
		i++;
	}
}

/*Encode the string to call web service*/
void encodeStringForHTTP(char *msg, char *mainMsg)
{
	int k = 0;

	for (int i = 0; i<strlen(mainMsg); i++)
	{
		if (mainMsg[i] == ' ')
		{
			msg[k++] = '%';
			msg[k++] = '2';
			msg[k++] = '0';
		}
		else if (mainMsg[i] == '\n')
		{
			msg[k++] = '%';
			msg[k++] = '0';
			msg[k++] = 'A';
		}
		else if (mainMsg[i] == '&')
		{
			msg[k++] = '%';
			msg[k++] = '2';
			msg[k++] = '6';
		}
		else if (mainMsg[i] == '#')
		{
			msg[k++] = '%';
			msg[k++] = '2';
			msg[k++] = '3';
		}
		else if (mainMsg[i] == '%')
		{
			msg[k++] = '%';
			msg[k++] = '2';
			msg[k++] = '5';
		}
		else if (mainMsg[i] == '?')
		{
			msg[k++] = '%';
			msg[k++] = '3';
			msg[k++] = 'F';
		}
		else if (mainMsg[i] == '/')
		{
			msg[k++] = '%';
			msg[k++] = '2';
			msg[k++] = 'F';
		}
		else if (mainMsg[i] == '+')
		{
			msg[k++] = '%';
			msg[k++] = '2';
			msg[k++] = 'B';
		}
		else
			msg[k++] = mainMsg[i];
	}
}

/*Initialize values from smpp.ini*/
void LoadConfig(char *src)
{
	char *token, *nextToken, ch;
	char tempString[300];
	iniFile ini;

	memset(&ini, MEMINIT_VALUE, sizeof(ini));
	parseConf(src, &ini, "smpp.ini");
	

	/********Get webservice url********/
	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "CDE_CURL_BATCH_SIZE", tempString);
	CDE_CURL_BATCH_SIZE = atoi(tempString);
	memset(tempString, MEMINIT_VALUE, sizeof(tempString));

	getParamValue(ini.p, ini.pCount, "CDE_FAILED_SMS", tempString);
	tempString[strlen(tempString) - 1] = 0;
	strcpy(CDE_FAILED_SMS, tempString);

	memset(CDE_URL, MEMINIT_VALUE, sizeof(CDE_URL));
	getParamValue(ini.p, ini.pCount, "CDE_URL", CDE_URL);
	strcat(CDE_URL, "\r\n");	

	/********NO reply string********/ 
	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "NO-REPLY-STRING", tempString);
	for (int i = 0; i < strlen(tempString); i++) if (tempString[i] == ',') NOREPLY_STRING_NUMBER++;
	if (NOREPLY_STRING_NUMBER > 0)
	{
		NOREPLY_STRING_NUMBER++;
		NOREPLY_STRINGS = (char **)malloc(NOREPLY_STRING_NUMBER);
		if (NOREPLY_STRING_NUMBER > 0)
		{
			token = STRTOK(tempString, ",", &nextToken);
			NOREPLY_STRINGS[0] = (char *)malloc(sizeof(char)* strlen(token));
			strcpy(NOREPLY_STRINGS[0], token);
			ConvertToUpper(NOREPLY_STRINGS[0]);

			for (int i = 1; i < NOREPLY_STRING_NUMBER; i++)
			{
				token = STRTOK(NULL, ",", &nextToken);
				NOREPLY_STRINGS[i] = (char *)malloc(sizeof(char)* strlen(token));
				strcpy(NOREPLY_STRINGS[i], token);
				ConvertToUpper(NOREPLY_STRINGS[i]);
			}
		}
	}

	
	memset(OPERATORINFO_URL, MEMINIT_VALUE, sizeof(OPERATORINFO_URL));
	getParamValue(ini.p, ini.pCount, "OPERATORINFO_URL", OPERATORINFO_URL);
	strcat(OPERATORINFO_URL, "\r\n");	

	memset(SMS_CLIENT_ID, MEMINIT_VALUE, sizeof(SMS_CLIENT_ID));
	getParamValue(ini.p, ini.pCount, "SMS_CLIENT_ID", SMS_CLIENT_ID);
	strcat(SMS_CLIENT_ID, "\r\n");		
	
	memset(SERVICECONFIG_URL, MEMINIT_VALUE, sizeof(SERVICECONFIG_URL));
	getParamValue(ini.p, ini.pCount, "SERVICECONFIG_CDE_URL", SERVICECONFIG_URL);
	strcat(SERVICECONFIG_URL, "\r\n");

	
	memset(SHORTCODE_URL, MEMINIT_VALUE, sizeof(SHORTCODE_URL));
	getParamValue(ini.p, ini.pCount, "SHORTCODE_URL", SHORTCODE_URL);

	strcat(SHORTCODE_URL, "\r\n");

	/********Get database related information********/
	memset(SERVER_NAME, MEMINIT_VALUE, sizeof(SERVER_NAME));
	getParamValue(ini.p, ini.pCount, "DATABASE_HOST", SERVER_NAME);

	memset(DB_USER, MEMINIT_VALUE, sizeof(DB_USER));
	getParamValue(ini.p, ini.pCount, "DATABASE_USERNAME", DB_USER);

	memset(DB_USERPASS, MEMINIT_VALUE, sizeof(DB_USERPASS));
	getParamValue(ini.p, ini.pCount, "DATABASE_PASSWROD", DB_USERPASS);

	memset(DB_NAME, MEMINIT_VALUE, sizeof(DB_NAME));
	getParamValue(ini.p, ini.pCount, "DATABASE_NAME", DB_NAME);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "TPS", tempString);
	TPS = atoi(tempString);


	/* Make Que or Fail thread executing flag initialisation */
	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "IS_REQUE_ENABLE", tempString);
	IS_REQUE_ENABLE = atoi(tempString);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "RETRYCOUNT", tempString);
	RETRYCOUNT = atoi(tempString);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "RETRY_DELAY", tempString);
	RETRY_DELAY = atoi(tempString);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "PING_BALL_DELAY", tempString);
	PING_BALL_DELAY = atoi(tempString);
	
	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "SHORTCODE_TPS", tempString);
	SHORTCODE_TPS = atoi(tempString);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "MAX_NUMBER_OF_ROW_FETCH", tempString);
	MAX_NUMBER_OF_ROW_FETCH = atoi(tempString);

	//
	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "APPEND_SHORTCODE_PREFIX", tempString);
	APPEND_SHORTCODE_PREFIX = atoi(tempString);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "SHORTCODE_PREFIX_VALUE", tempString);
	SHORTCODE_PREFIX_VALUE = atoi(tempString);
	//

	memset(LOG_DIRNAME, MEMINIT_VALUE, sizeof(LOG_DIRNAME));
	getParamValue(ini.p, ini.pCount, "LOG_DIRECTORY_NAME", LOG_DIRNAME);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "SMSLOG_ENABLED", tempString);
	SMSLOG_ENABLED = atoi(tempString);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "LOG_LEVEL", tempString);
	LOG_LEVEL = atoi(tempString);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "LOG_DESTINATION", tempString);
	LOG_DESTINATION = atoi(tempString);

	memset(LOG_HOST, MEMINIT_VALUE, sizeof(LOG_HOST));
	getParamValue(ini.p, ini.pCount, "LOG_HOST", LOG_HOST);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "LOG_PORT", tempString);
	LOG_PORT = atoi(tempString);

	memset(LOG_COMPONENT, MEMINIT_VALUE, sizeof(LOG_COMPONENT));
	getParamValue(ini.p, ini.pCount, "LOG_COMPONENT", LOG_COMPONENT);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "USE_DEFAULT_STATUS", tempString);
	USE_DEFAULT_STATUS = atoi(tempString);	

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "NUMBER_SPECIFIC_LOG_ENABLE", tempString);
	NUMBER_SPECIFIC_LOG_ENABLE = atoi(tempString);

	memset(NUMBER1, MEMINIT_VALUE, sizeof(NUMBER1));
	getParamValue(ini.p, ini.pCount, "NUMBER1", NUMBER1);

	memset(NUMBER2, MEMINIT_VALUE, sizeof(NUMBER2));
	getParamValue(ini.p, ini.pCount, "NUMBER2", NUMBER2);

	memset(INTERIM_STATUS, MEMINIT_VALUE, sizeof(INTERIM_STATUS));
	getParamValue(ini.p, ini.pCount, "INTERIM_STATUS", INTERIM_STATUS);

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "COMMAND_PROCESS_PORT_CDE", tempString);
	COMMAND_PROCESS_PORT = atoi(tempString);	

	memset(tempString, MEMINIT_VALUE, sizeof(tempString));
	getParamValue(ini.p, ini.pCount, "MAX_SMS_IN_INBOX_HANDLER_QUEUE", tempString);
	MAX_SMS_IN_QUE = atoi(tempString);
}

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{	
	size_t realsize = size * nmemb;

	logActivity(__FUNCTION__, "curl_cb", E_DEBUG_MINOR, "contents: %s| size: %d | nmemb: %d", (char*)contents, size, nmemb);

	if (NULL != contents)
	{		
		struct httpResponseBuf *mem = (struct httpResponseBuf *)userp;
		if (realsize > sizeof(mem->buf))
		{
			memcpy(&(mem->buf[mem->size]), contents, sizeof(mem->buf));			
		}
		else
		{			
			memcpy(&(mem->buf[mem->size]), contents, realsize);			
		}

		mem->size += realsize;
		logActivity(__FUNCTION__, "curl_cb", E_DEBUG_MINOR, "mem->size %d", mem->size);
		mem->buf[mem->size] = 0;
	}	

	return realsize;
}

static size_t WriteBufferCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;

	struct cdebuffer *mem = (struct cdebuffer *)userp;

	if (realsize > sizeof(mem->buf))
		memcpy(&(mem->buf[mem->size]), contents, sizeof(mem->buf));
	else
		memcpy(&(mem->buf[mem->size]), contents, realsize);

	mem->size += realsize;
	mem->buf[mem->size] = 0;
	return realsize;
}

/*Interact with command user at other end to set configuration*/
void *handleClient(void *ptr)
{
	SOCKET *s;
	char msg[MAXCOMMANDLENGTH];
	char reply[MAXCOMMANDLENGTH];
	char log[1500];
	time_t counter;
	struct tm timer;

	s = (SOCKET *)ptr;

	while (1)
	{
		memset(msg, MEMINIT_VALUE, sizeof(msg));
		memset(reply, MEMINIT_VALUE, sizeof(reply));
		int ret;
		ret = recv(*s, msg, MAXCOMMANDLENGTH, 0);

		ConvertToUpper(msg);
		if (strlen(msg) != 0) printf("%s\n", msg);

		if (strcmp(msg, "CURRENTTPS") == 0)
		{
			logActivity(__FUNCTION__, "cmd", E_INFO, "TPS REQUESTED");
			SPRINTF(reply, sizeof(reply), "TPS is: %d", TPS);
			ret = send(*s, reply, sizeof(reply), 0);
			if (ret <= 0)
			{
				SPRINTF(log, sizeof(log), "Sending failed, exiting thread....");
				logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
				closesocket(*s);
				break;
			}
			else
			{
				SPRINTF(log, sizeof(log), "SENT REPLY ------%s------", reply);
				logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
			}
		}
		if (strcmp(msg, "CHANGETPS") == 0)
		{
			SPRINTF(log, sizeof(log), "CHANGE IN TPS REUQESTED");
			logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
			SPRINTF(reply, sizeof(reply), "PLEASE TYPE NEW TPS AND SEND");
			ret = send(*s, reply, sizeof(reply), 0);
			if (ret <= 0)
			{
				SPRINTF(log, sizeof(log), "Sending failed, exiting thread....");
				logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
				closesocket(*s);
				break;
			}
			else
			{
				memset(msg, MEMINIT_VALUE, sizeof(msg));

				SPRINTF(log, sizeof(log), "SENT REPLY ------%s------", reply);
				logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
				ret = recv(*s, msg, MAXCOMMANDLENGTH, 0);

				int newTPS;
				if ((newTPS = atoi(msg)) == 0)
				{
					//send reply as error
					SPRINTF(reply, sizeof(reply), "Connot convert TPS into interger, please try again");
					ret = send(*s, reply, sizeof(reply), 0);
				}
				else
				{
					//confirm that TPS has been changed
					memcpy(&TPS, &newTPS, sizeof(TPS));
					SUBMISSION_DELAY = (1000 / TPS) * OUTBOX_THREAD_COUNT;

					SPRINTF(reply, sizeof(reply), "New TPS is:%d", TPS);
					ret = send(*s, reply, sizeof(reply), 0);
				}

			}
		}
		if (strcmp(msg, "TPSPREVHOUR") == 0)
		{
			SPRINTF(log, sizeof(log), "TPS OF PREVIOUS HOUR REQUESTED");
			logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);

			time(&counter);
			timer = *localtime(&counter);

			int i, j, k;
			i = timer.tm_hour;
			if (i == 0) i = 24;
			i = i - 1;
			for (j = timer.tm_min; j < 60; j++)
			for (k = timer.tm_sec; k < 60; k++)
			{
				SPRINTF(reply, sizeof(reply), "AT %d:%d:%d, TPS WAS:%d\n", i, j, k, TPS_RECORDS[i][j][k]);
				ret = send(*s, reply, strlen(reply), 0);
			}

			i = timer.tm_hour;
			for (j = 0; j <= timer.tm_min; j++)
			for (k = 0; k < timer.tm_sec; k++)
			{
				SPRINTF(reply, sizeof(reply), "AT %d:%d:%d, TPS WAS:%d\n", i, j, k, TPS_RECORDS[i][j][k]);
				ret = send(*s, reply, strlen(reply), 0);
			}
		}
		/*
		else if (strcmp(msg, "GETTPSRECORD") == 0 || strcmp(msg, "GETTPSRECORDS") == 0)
		{
		SPRINTF(log, sizeof(log), "PREVIOUS TPS RECORDS REQUESTED");
		logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
		int count;

		for (count = 1; count < 61; count++)
		{
		char temp[20];
		SPRINTF(temp, sizeof(temp), "At Second[%d]:%d", (CURRENT_SECOND_FOR_TPS % 60) + 1, TPS_RECORDS[(CURRENT_SECOND_FOR_TPS % 60) + 1]);

		ret = send(*s, temp, sizeof(temp), 0);
		if (ret <= 0)
		{
		SPRINTF(log, sizeof(log), "Sending failed, exiting thread....");
		logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
		closesocket(*s);
		break;
		}
		else logActivity(__FUNCTION__, "cmd", E_INFO, "SENT REPLY ------%s------", temp);
		}

		SPRINTF(reply, sizeof(reply), "Last entry is the latest TPS");
		ret = send(*s, reply, sizeof(reply), 0);

		if (ret <= 0)
		{
		logActivity(__FUNCTION__, "cmd", E_INFO, "Sending failed, exiting thread....");
		closesocket(*s);
		break;
		}
		else logActivity(__FUNCTION__, "cmd", E_INFO, "SENT REPLY ------%s------", reply);
		}
		*/
		else if (strcmp(msg, "Q") == 0 || strcmp(msg, "QUIT") == 0)
		{
			logActivity(__FUNCTION__, "cmd", E_INFO, "Exiting command found, terminating connection...");
			closesocket(*s);
			break;
		}
		else
		{
			SPRINTF(reply, sizeof(reply), "Command not found");
			ret = send(*s, reply, sizeof(reply), 0);
			if (ret <= 0)
			{
				SPRINTF(log, sizeof(log), "Sending failed, exiting thread....");
				logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
				closesocket(*s);
				break;
			}
			else
			{
				SPRINTF(log, sizeof(log), "SENT REPLY ------%s------", reply);
				logActivity(__FUNCTION__, "cmd", E_INFO, "%s", log);
			}
		}
		Sleep(MIN_SLEEP);
	}

	closesocket(*s);
	return NULL;

}

/*Command process to remotely access the smppClient*/
void *commandProcess(void *ptr) {

	pthread_t th;
	SOCKET	sccl, sc;
	sockaddr_in from;
	int fromlen = sizeof(from);

#ifndef COMPILER_GCC
	WSADATA	ws;
	if (WSAStartup(0x101, &ws) != 0){	//For windows, this lines are needed. Flushes the memory
		//reportActivity("-ERR: Connect  WSA Initialization Failed", "Function: InitSock");
		logActivity(__FUNCTION__, "cmd", E_INFO, "Socket initialization in windows environment");
	}
#endif

	sc = socket(AF_INET, SOCK_STREAM, 0);
	if (sc == INVALID_SOCKET) {
		logActivity(__FUNCTION__, "cmd", E_INFO, "Invalid socket created..!!");
	}

	from.sin_port = htons(COMMAND_PROCESS_PORT);
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sc, (sockaddr *)(&from), sizeof(from)) != 0) {
		//reportActivity("-ERR: Bind Failed..!!", "Function: InitSock");	
		logActivity(__FUNCTION__, "cmd", E_INFO, "Binding failed!!");
	}

	if (listen(sc, 5) != 0) {
		logActivity(__FUNCTION__, "cmd", E_INFO, "LISTENING failed!");
	}

	while (true) {

		if ((sccl = accept(sc, (struct sockaddr*)&from, (socklen_t*)&fromlen)) != 0) {
			logActivity(__FUNCTION__, "cmd", E_INFO, "Client connected..!!");
			pthread_create(&th, NULL, handleClient, &sccl);
		}
		else {
			logActivity(__FUNCTION__, "cmd", E_INFO, "Client connection failed..!!");
		}
	}

	return NULL;
}

pid_t proc_find(const char* name) 
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }

    }

    closedir(dir);
    return -1;
}

bool isAnotherInstanceRunning()
{
	bool bRet = false;
	FILE *pPipe;
	char psBuffer[128];
	char  command[50];	
	
	// create the command to find process name
	sprintf(command, "ps -e | grep %s", __progname);
	
	/* Open the command for reading. */
	pPipe = popen(command, "r");
	if (NULL == pPipe) 
	{
		printf(__FUNCTION__, "%s", "Failed to run command\n" );
		exit(1);
	}

	int instanceCount = 0;
	/* Read the output a line at a time - output it. */
	while (fgets(psBuffer, sizeof(psBuffer)-1, pPipe) != NULL) 
	{		
		printf("%s", psBuffer);
		instanceCount++;
	}
		
	pclose(pPipe);
	
	if(instanceCount > 1)
	{
		bRet = true;
	}
	
	return bRet;
}
 
int urlencode(char *dest, const char *src) {
    /* urlencode all non-alphanumeric characters in the C-string 'src'
       store result in the C-string 'dest'
       return the length of the url encoded C-string
    */
    char *d;
    int i;
    for(i=0, d=dest; src[i]; i++) {
        if(isalnum(src[i])) *(d++) = src[i];
        else {
            sprintf(d, "%%%02X", src[i]);
            d += 3;
        }
    }   
    *d = 0;
    return d-dest;
}

#endif //_COMMON_H_


