#ifndef INBOX_HANDLER_H_
#define INBOX_HANDLER_H_

//#define COMPILER_GCC
//#ifdef COMPILER_GCC


#include "ipcQueueHandler.h"


void initializeMemoryQue()
{
	initializeIPCQ();
}

void RemoveMemoryQue()
{
	RemoveIPCQ();
}

bool isInboxSingleMemoryQueOverloaded()
{
	struct msqid_ds msqid_ds;
	int ret = msgctl(SMS_INBOX_QUE_SHMID, IPC_STAT,&msqid_ds);
	bool bRet = false;
	if(-1 != ret)
	{
		int  num = msqid_ds.msg_qnum;

		//if(num > (MAX_SMS_IN_QUE - MAX_NUMBER_OF_ROW_FETCH))
		if(num > MAX_SMS_IN_QUE)
		{
			logActivity(__FUNCTION__, "IPC_QUE", E_DEBUG_MAJOR, " SMS_INBOX_QUE: Too Many SMS");
			bRet = true;
		}
	}
	else
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS_INBOX_QUE IPC_STAT Error");
	}

	return bRet;
}

bool isInboxSingleMemoryQueEmpty()
{
	struct msqid_ds msqid_ds;
	int ret = msgctl(SMS_INBOX_QUE_SHMID, IPC_STAT,&msqid_ds);
	bool bRet = false;
	if(-1 != ret)
	{
		if(0 == msqid_ds.msg_qnum)
		{			
			bRet = true;
		}
	}
	else
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS_INBOX_QUE IPC_STAT Error");
	}

	return bRet;
}

void insertIntoInboxSingleMemoryQue(smsinbox smsEntry)
{
	insert_into_inbox_single_queue(SMS_INBOX_QUE_SHMID, smsEntry);
}

bool isInboxResponseMemoryQueOverloaded()
{
	struct msqid_ds msqid_ds;
	int ret = msgctl(SMS_INBOX_RESPONSE_QUE_SHMID, IPC_STAT,&msqid_ds);
	bool bRet = false;
	if(-1 != ret)
	{
		int  num = msqid_ds.msg_qnum;

		//if(num > (MAX_SMS_IN_QUE*10 - MAX_NUMBER_OF_ROW_FETCH))
		if(num > MAX_SMS_IN_QUE)
		{
			logActivity(__FUNCTION__, "IPC_QUE", E_DEBUG_MAJOR, " SMS_INBOX_RESPONSE_QUE: Too Many SMS");
			bRet = true;
		}
	}
	else
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS_INBOX_RESPONSE_QUE IPC_STAT Error");
	}

	return bRet;
}

bool isInboxResponseMemoryQueEmpty()
{
	struct msqid_ds msqid_ds;
	int ret = msgctl(SMS_INBOX_RESPONSE_QUE_SHMID, IPC_STAT,&msqid_ds);
	bool bRet = false;
	if(-1 != ret)
	{
		if(0 == msqid_ds.msg_qnum)
		{			
			bRet = true;
		}
	}
	else
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS_INBOX_RESPONSE_QUE IPC_STAT Error");
	}

	return bRet;
}

void insertIntoInboxResponseMemoryQue(smsinbox smsEntry)
{
	insert_into_inbox_response_queue(SMS_INBOX_RESPONSE_QUE_SHMID, smsEntry);
}

bool isCDEFailedMemoryQueOverloaded()
{
	struct msqid_ds msqid_ds;
	int ret = msgctl(SMS_CDE_FAILED_QUE_SHMID, IPC_STAT,&msqid_ds);
	bool bRet = false;
	if(-1 != ret)
	{
		int  num = msqid_ds.msg_qnum;

		//if(num > (MAX_SMS_IN_QUE*5 - MAX_NUMBER_OF_ROW_FETCH))
		if(num > MAX_SMS_IN_QUE)
		{
			logActivity(__FUNCTION__, "IPC_QUE", E_DEBUG_MAJOR, " SMS_CDE_FAILED_QUE: Too Many SMS");
			bRet = true;
		}
	}
	else
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS_CDE_FAILED_QUE IPC_STAT Error");
	}

	return bRet;
}

bool isCDEFailedMemoryQueEmpty()
{
	struct msqid_ds msqid_ds;
	int ret = msgctl(SMS_CDE_FAILED_QUE_SHMID, IPC_STAT,&msqid_ds);
	bool bRet = false;
	if(-1 != ret)
	{
		if(0 == msqid_ds.msg_qnum)
		{			
			bRet = true;
		}
	}
	else
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS_CDE_FAILED_QUE IPC_STAT Error");
	}

	return bRet;
}

void insertIntoCDEFailedMemoryQue(cde_failed smsEntry)
{
	insert_into_cde_failed_queue(SMS_CDE_FAILED_QUE_SHMID, smsEntry);
}

bool isOutboxSingleMemoryQueOverloaded()
{
	struct msqid_ds msqid_ds;
	int ret = msgctl(SMS_OUTBOX_QUE_SHMID, IPC_STAT,&msqid_ds);
	bool bRet = false;
	if(-1 != ret)
	{
		int  num = msqid_ds.msg_qnum;

		//if(num > (MAX_SMS_IN_QUE*10 - MAX_NUMBER_OF_ROW_FETCH))
		if(num > MAX_SMS_IN_QUE)
		{
			logActivity(__FUNCTION__, "IPC_QUE", E_DEBUG_MAJOR, " SMS_OUTBOX_QUE: Too Many SMS");
			bRet = true;
		}
	}
	else
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS_OUTBOX_QUE IPC_STAT Error");
	}

	return bRet;
}

bool isOutboxSingleMemoryQueEmpty()
{
	struct msqid_ds msqid_ds;
	int ret = msgctl(SMS_OUTBOX_QUE_SHMID, IPC_STAT,&msqid_ds);
	bool bRet = false;
	if(-1 != ret)
	{
		if(0 == msqid_ds.msg_qnum)
		{			
			bRet = true;
		}
	}
	else
	{
		logActivity(__FUNCTION__, "IPC_QUE", E_ERROR, " SMS_OUTBOX_QUE IPC_STAT Error");
	}

	return bRet;
}


void insertIntoOutboxSingleMemoryQue(smsoutbox smsEntry)
{
	insert_into_outbox_single_queue(SMS_OUTBOX_QUE_SHMID, smsEntry);
}

//int getFromInboxSingleMemoryQueue(smsinbox_queue* queue)
//{
//	int ret =  msgrcv(SMS_INBOX_QUE_SHMID, queue, sizeof(struct smsinbox_queue) - sizeof(long), SMS_INBOX_QUE_TYPE, 0);
//
//	return ret;
//}

int getFromInboxSingleMemoryQueue(smsinbox* sms)
{
	smsinbox_queue queue;
	int ret =  msgrcv(SMS_INBOX_QUE_SHMID, &queue, sizeof(struct smsinbox_queue) - sizeof(long), SMS_INBOX_QUE_TYPE, IPC_NOWAIT);

	memcpy(sms, &(queue.sms), sizeof(struct smsinbox));

	return ret;
}

//int getFromInboxResonseMemoryQueue(smsinbox_queue* queue)
//{
//	int ret =  msgrcv(SMS_INBOX_RESPONSE_QUE_SHMID, queue, sizeof(struct smsinbox_queue) - sizeof(long), SMS_INBOX_RESPONSE_QUE_TYPE, 0);
//
//	return ret;
//}


int getFromInboxResonseMemoryQueue(smsinbox* sms)
{
	smsinbox_queue queue;
	int ret =  msgrcv(SMS_INBOX_RESPONSE_QUE_SHMID, &queue, sizeof(struct smsinbox_queue) - sizeof(long), SMS_INBOX_RESPONSE_QUE_TYPE, IPC_NOWAIT);

	memcpy(sms, &(queue.sms), sizeof(struct smsinbox));
	return ret;
}

//int getFromCDEFailedMemoryQueue(cde_failed_queue* queue)
//{
//	int ret =  msgrcv(SMS_CDE_FAILED_QUE_SHMID, queue, sizeof(struct cde_failed_queue) - sizeof(long), SMS_CDE_FAILED_QUE_TYPE, 0);
//
//	return ret;
//}

int getFromCDEFailedMemoryQueue(cde_failed* sms)
{
	cde_failed_queue queue;
	int ret =  msgrcv(SMS_CDE_FAILED_QUE_SHMID, &queue, sizeof(struct cde_failed_queue) - sizeof(long), SMS_CDE_FAILED_QUE_TYPE, IPC_NOWAIT);

	memcpy(sms, &(queue.sms), sizeof(struct cde_failed));

	return ret;
}

//int getFromOutboxMemoryQueue(smsoutbox_queue* queue)
//{
//	int ret =  msgrcv(SMS_OUTBOX_QUE_SHMID, queue, sizeof(struct smsoutbox_queue) - sizeof(long), SMS_OUTBOX_QUE_TYPE, 0);
//
//	return ret;
//}

int getFromOutboxMemoryQueue(smsoutbox* sms)
{
	smsoutbox_queue queue;
	int ret =  msgrcv(SMS_OUTBOX_QUE_SHMID, &queue, sizeof(struct smsoutbox_queue) - sizeof(long), SMS_OUTBOX_QUE_TYPE, 0);

	memcpy(sms, &(queue.sms), sizeof(struct smsoutbox));

	return ret;
}


//#endif //COMPILER_GCC




#endif //INBOX_HANDLER_H_

