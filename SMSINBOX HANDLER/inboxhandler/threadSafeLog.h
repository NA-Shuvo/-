/*
* threadSafeLog.h
*
*  Created on: Mar 31, 2016
*      Author: Raj!b
*/

#ifndef _THREADSAFELOG_H
#define _THREADSAFELOG_H


#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <interop.h>
#include <SocketFramework.h>
#include <smpplib.h>
#include <mysql.h>

//#define LOG_ACTIVITY_TYPE 6
#define LOG_SUBQUEUE 1
#define LOG_SHKEY	1245
#define  LOG_SIZE 10000
int LOG_SHMID = -1;

bool LOG_ENABLED = false;

// Structure for Log

typedef struct log{
	char src[50];
	char prefix[20];
	int logLevel;
	char msg[LOG_SIZE];
} log;

struct lqueue{
	long ltype;
	log logque;
};

int insertLogIntoQue(const char *src, const char *prefix, int loglevel, char *msg, ...)
{
	int msgid = -1;

	if ((msgid = msgget(LOG_SHKEY, 0666)) < 0) {
		return 0;
	}	

	if(false == LOG_ENABLED)
	{
		return 0;
	}
	if (NULL == msg || NULL == src || NULL == prefix )
	{
		return 0;
	}

	int rsp;
	lqueue lq;
	char logBuf[LOG_SIZE] = {0};
	va_list args;

	va_start(args, msg);
	int n = _vscprintf(msg, args) + 1;
	if (n < LOG_SIZE)
		VSPRINTF(logBuf, (size_t)LOG_SIZE, msg, args);
	else
		SPRINTF(logBuf, LOG_SIZE, "Activity Log Size is greater than log buffer..!");
	va_end(args);

	memset(&lq, 0, sizeof(lq));
	lq.ltype = LOG_SUBQUEUE;
	//strcpy(lq.logque.src, src);
	SPRINTF(lq.logque.src, sizeof(lq.logque.src), src);
	//strcpy(lq.logque.prefix , prefix);
	SPRINTF(lq.logque.prefix, sizeof(lq.logque.prefix), prefix);
	lq.logque.logLevel = loglevel;
	//strcpy(lq.logque.msg, logBuf);
	SPRINTF(lq.logque.msg, sizeof(lq.logque.msg), logBuf);
	//printf("src %s, prefix %s, loglevel %d, msg %s\n", lq.logque.src, lq.logque.prefix, lq.logque.logLevel, lq.logque.msg);

	rsp = msgsnd(LOG_SHMID, &lq, sizeof(struct lqueue) - sizeof(long), 0);	
	if (rsp < 0)
	{		
		logActivity(__FUNCTION__, "ipcq", E_ERROR, "Inserting in LOG QUEUE problem id: %d, key: %d", LOG_SHMID, LOG_SHKEY);		
	}

	return 0;
}

void initializeIPCQForLog() 
{
	char msg[LOG_SIZE];
	LOG_SHMID = -1;
	LOG_ENABLED = true;
	
	if((LOG_SHMID = msgget(LOG_SHKEY, IPC_CREAT | 0666)) < 0 ) 
	{ 		
		logActivity(__FUNCTION__, "ipcq", E_ERROR, "LOG Queue, shared memory id: %d, key: %d", LOG_SHMID, LOG_SHKEY);		
		exit(1);
	}	
	Sleep(1000);
	
}

void *writeLogActivity(void *p) {

	char thName[20];			// should be less than 10 char
	int *id;
	id = (int*) p;
	lqueue l;
	int nRet;
	
	//printf("writeLogActivity\n");

	while (true)
	{		
		nRet = -1; 
		nRet = msgrcv(LOG_SHMID, &l, sizeof(struct lqueue) - sizeof(long), LOG_SUBQUEUE, IPC_NOWAIT);
		if (nRet >= 0)
		{
			logActivity(l.logque.src, "CDEHndlr", l.logque.logLevel, l.logque.msg);
		}
		Sleep(MIN_SLEEP);
	}
	return NULL;
}

#endif //_THREADSAFELOG_H
