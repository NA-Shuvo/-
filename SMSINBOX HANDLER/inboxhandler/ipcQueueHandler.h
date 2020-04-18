/*
 * ipcQueueHandler.h
 *
 *  Created on: Aug 11, 2015
 *      Author: root
 */

#ifndef IPC_QUE_HANDLER_H_
#define IPC_QUE_HANDLER_H_


#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int SMS_INBOX_QUE_SHMID = -1;
int SMS_INBOX_RESPONSE_QUE_SHMID = -1;
int SMS_CDE_FAILED_QUE_SHMID = -1;
int SMS_OUTBOX_QUE_SHMID = -1;

enum IPC_QUE_TYPE
{
	SMS_INBOX_QUE_TYPE = 1,
	SMS_INBOX_RESPONSE_QUE_TYPE,
	SMS_CDE_FAILED_QUE_TYPE,
	SMS_OUTBOX_QUE_TYPE
};

enum IPC_QUE_SHKEY_TYPE
{
	SMS_INBOX_QUE_SHKEY = 0x2345,
	SMS_INBOX_RESPONSE_QUE_SHKEY,
	SMS_CDE_FAILED_QUE_SHKEY,
	SMS_OUTBOX_QUE_SHKEY
};

struct smsinbox_queue {
	long mtype;
	smsinbox sms;
} ;

struct cde_failed_queue {
	long mtype;
	cde_failed sms;
} ;

struct smsoutbox_queue {
	long mtype;
	smsoutbox sms;
} ;

//key_t SMS_INBOX_QUE_SHKEY, SMS_INBOX_RESPONSE_QUE_SHKEY, SMS_CDE_FAILED_QUE_SHKEY,SMS_OUTBOX_QUE_SHKEY;

void initializeIPCQ(){

	//key_t key;
	//key=ftok("/opt/share/ss2.txt",'T');
	//SMS_INBOX_QUE_SHKEY = ftok("/opt/share/SMS_INBOX_QUE_SHKEY.txt",'A');
	//SMS_INBOX_RESPONSE_QUE_SHKEY = ftok("/opt/share/SMS_INBOX_RESPONSE_QUE_SHKEY.txt",'B');
	//SMS_CDE_FAILED_QUE_SHKEY = ftok("/opt/share/SMS_CDE_FAILED_QUE_SHKEY.txt",'C');
	//SMS_OUTBOX_QUE_SHKEY = ftok("/opt/share/SMS_OUTBOX_QUE_SHKEY.txt",'D');

	//SMS_INBOX_RESPONSE_QUE_SHKEY, SMS_CDE_FAILED_QUE_SHKEY,SMS_OUTBOX_QUE_SHKEY;
	// see if the memory exists and print it if so

	if ( (SMS_INBOX_QUE_SHMID = msgget(SMS_INBOX_QUE_SHKEY, IPC_CREAT | 0666)) < 0 )
	{
		//perror("msgget");
		printf( "SMS inbox queue Error shmid = %d\n", SMS_INBOX_QUE_SHMID );
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS INBOX QUE ERROR | SMS_INBOX_QUE_SHMID = %d", SMS_INBOX_QUE_SHMID);
		exit(1);
	}
	logActivity(__FUNCTION__, "IPC_QUE", E_INFO, "SMS INBOX Initialize | SMS_INBOX_QUE_SHMID = %d", SMS_INBOX_QUE_SHMID);

	if ( (SMS_INBOX_RESPONSE_QUE_SHMID = msgget(SMS_INBOX_RESPONSE_QUE_SHKEY, IPC_CREAT | 0666)) < 0 )
	{
		//perror("msgget");
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS INBOX UPDATE QUE ERROR | SMS_OUTBOX_QUE_SHMID = %d", SMS_INBOX_RESPONSE_QUE_SHMID);
		exit(1);
	}
	//printf( "Initialize SMS inbox update queue shmid = %d\n", SMS_INBOX_RESPONSE_QUE_SHMID );
	logActivity(__FUNCTION__, "IPC_QUE", E_INFO, "SMS INBOX UPDATE Initialize | SMS_INBOX_RESPONSE_QUE_SHMID = %d", SMS_INBOX_RESPONSE_QUE_SHMID);

	if ( (SMS_OUTBOX_QUE_SHMID = msgget(SMS_OUTBOX_QUE_SHKEY, IPC_CREAT | 0666)) < 0 )
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS OUTBOX QUE ERROR | SMS_OUTBOX_QUE_SHMID = %d", SMS_OUTBOX_QUE_SHMID);
		exit(1);
	}
	logActivity(__FUNCTION__, "IPC_QUE", E_INFO, "SMS OUTBOX Initialize | SMS_OUTBOX_QUE_SHMID = %d", SMS_OUTBOX_QUE_SHMID);

	if ( (SMS_CDE_FAILED_QUE_SHMID = msgget(SMS_CDE_FAILED_QUE_SHKEY, IPC_CREAT | 0666)) < 0 )
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS CDE FAILED QUE ERROR | SMS_CDE_FAILED_QUE_SHMID = %d", SMS_CDE_FAILED_QUE_SHMID);
		exit(1);
	}
	logActivity(__FUNCTION__, "IPC_QUE", E_INFO, "SMS CDE Failed Initialize | SMS_CDE_FAILED_QUE_SHMID = %d", SMS_CDE_FAILED_QUE_SHMID);

}

void RemoveIPCQ()
{
	// delete it
	if ( shmctl (SMS_INBOX_QUE_SHMID, IPC_RMID, NULL) < 0 )
	{
		perror("SMS_INBOX_QUE_SHMID");
		exit(1);
	}

	// delete it
	if ( shmctl (SMS_INBOX_RESPONSE_QUE_SHMID, IPC_RMID, NULL) < 0 )
	{
		perror("SMS_INBOX_RESPONSE_QUE_SHMID");
		exit(1);
	}

	// delete it
	if ( shmctl (SMS_OUTBOX_QUE_SHMID, IPC_RMID, NULL) < 0 )
	{
		perror("SMS_OUTBOX_QUE_SHMID");
		exit(1);
	}
}

void insert_into_inbox_single_queue(int shmid, smsinbox smsEntry) {

	smsinbox_queue m;
	m.mtype = SMS_INBOX_QUE_TYPE;

	memcpy(&m.sms, &smsEntry, sizeof(struct smsinbox));

	int ret = msgsnd(shmid,&m,sizeof(struct smsinbox_queue)-sizeof(long),0);
	if(-1 != ret)
	{		
		logActivity(__FUNCTION__, "IPC_QUE", E_DEBUG_MAJOR, " Message ID: %d, Message: %s, SHMID = %d", m.sms.msgID,m.sms.msg, shmid);
	}
	else
	{		
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " Message ID: %d, SHMID = %d", m.sms.msgID, shmid);
	}
}

void insert_into_inbox_response_queue(int shmid, smsinbox smsEntry) {

	smsinbox_queue m;
	m.mtype = SMS_INBOX_RESPONSE_QUE_TYPE;

	memcpy(&m.sms, &smsEntry, sizeof(struct smsinbox));
	
	int ret = msgsnd(shmid,&m,sizeof(struct smsinbox_queue)-sizeof(long),0);
	if(-1 != ret)
	{		
		logActivity(__FUNCTION__, "IPC_QUE", E_DEBUG_MAJOR, " Message ID: %d, SHMID = %d", m.sms.msgID, shmid);
	}
	else
	{		
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " Message ID: %d, SHMID = %d", m.sms.msgID, shmid);
	}
}

void insert_into_cde_failed_queue(int shmid, cde_failed smsEntry)
{
	cde_failed_queue m;
	m.mtype = SMS_CDE_FAILED_QUE_TYPE;

	memcpy(&m.sms, &smsEntry, sizeof(struct cde_failed));

	int ret = msgsnd(shmid,&m,sizeof(struct cde_failed_queue)-sizeof(long),0);
	if(-1 != ret)
	{		
		logActivity(__FUNCTION__, "IPC_QUE", E_DEBUG_MAJOR, " Message ID: %d, SHMID = %d", m.sms.msgID, shmid);
	}
	else
	{		
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " Message ID: %d, SHMID = %d", m.sms.msgID, shmid);
	}
}

void insert_into_outbox_single_queue(int shmid, smsoutbox smsEntry) {

	smsoutbox_queue m;
	m.mtype = SMS_OUTBOX_QUE_TYPE;

	memcpy(&m.sms, &smsEntry, sizeof(struct smsoutbox));
	
	int ret = msgsnd(shmid,&m,sizeof(struct smsoutbox_queue)-sizeof(long),0);
	if(-1 != ret)
	{		
		logActivity(__FUNCTION__, "IPC_QUE", E_DEBUG_MAJOR, " SHMID = %d Message refID = %s",  shmid, smsEntry.refID);
	}
	else
	{		
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SHMID = %d Message refID = %s",  shmid, smsEntry.refID);
	}
}


#endif // IPC_QUE_HANDLER_H_
