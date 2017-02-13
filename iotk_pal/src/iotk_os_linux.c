#if defined(__linux__)
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <stdio.h>

//#include "osi/osi_ext.h"
#include "iotk/iotk_pal.h"
//////////////////////////////////////////////////////////////////////////
//
// MsgQ
//
typedef struct OSI_MSGQ
{
    pthread_mutex_t guard;
    sem_t queue;
    int rpos;
    int wpos;
    size_t count;
    size_t msg_size;
    size_t max_msgs;
} OSI_MSGQ;


static struct timespec *timespec_delay(struct timespec *t, unsigned int ms)
{
    unsigned int usec;
    struct timeval now;

    gettimeofday(&now, NULL);
    usec = (ms % 1000) * 1000 + now.tv_usec;
    t->tv_sec = now.tv_sec + ms / 1000 + usec / 1000000;
    t->tv_nsec = usec % 1000000 * 1000;
    return t;
}

iotk_res_t iotk_msgq_create(iotk_msgq_t *pMsgQ,
                              char *pMsgQName,
                              unsigned long MsgSize,
                              unsigned long MaxMsgs)
{
    int err;
    OSI_MSGQ *q;
    iotk_res_t res;

    // unreferenced param
    pMsgQName = pMsgQName;

    if (MsgSize == 0 || MaxMsgs == 0)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = (OSI_MSGQ *)malloc(sizeof(OSI_MSGQ) + MsgSize * MaxMsgs);

    if (NULL == q)
    {
        return IOTK_RES_MEMORY_ALLOCATION_FAILURE;
    }

    memset(q, 0, sizeof(OSI_MSGQ) + MsgSize * MaxMsgs);

    q->msg_size = MsgSize;
    q->max_msgs = MaxMsgs;

    err = sem_init(&q->queue, 0, q->max_msgs);

    if (0 != err)
    {
        free(q);
        return IOTK_RES_OPERATION_FAILED;
    }

    err = pthread_mutex_init(&q->guard, NULL);

    if (0 != err)
    {
        sem_destroy(&q->queue);
        free(q);
        return IOTK_RES_OPERATION_FAILED;
    }

    *pMsgQ = q;
    return IOTK_RES_OK;
}

iotk_res_t iotk_msgq_delete(iotk_msgq_t* pMsgQ)
{
    OSI_MSGQ *q;

    if (NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(OSI_MSGQ **)pMsgQ;

    if (NULL == q)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    sem_destroy(&q->queue);
    pthread_mutex_destroy(&q->guard);
    free(q);

    return IOTK_RES_OK;
}

iotk_res_t iotk_msgq_write(iotk_msgq_t* pMsgQ, void* pMsg , iotk_time_t Timeout)
{
    int err;
    char *wpointer = NULL;
    struct timespec delay;

    OSI_MSGQ *q;
    iotk_res_t res = IOTK_RES_FAILURE;

    if (NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(OSI_MSGQ **)pMsgQ;

    if (NULL == q)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    err = pthread_mutex_timedlock(&q->guard, timespec_delay(&delay, Timeout));
    if (0 == err)
    {
        //if (0 == sem_timedwait(&q->queue, timespec_delay(&delay, Timeout)))
        {
            wpointer = (char *)&q[1] + q->wpos * q->msg_size;
            memcpy(wpointer, pMsg, q->msg_size);
            q->wpos = (++q->wpos) % q->max_msgs;
            ++q->count;
            res = IOTK_RES_OK;

			sem_post(&q->queue);
        }
    }
    pthread_mutex_unlock(&q->guard);
    return res;
}

iotk_res_t iotk_msgq_read(iotk_msgq_t* pMsgQ, void* pMsg , iotk_time_t Timeout)
{
    int err;
    char *rpointer = NULL;
    struct timespec delay;

    OSI_MSGQ *q;
    iotk_res_t res = IOTK_RES_FAILURE;

    if (NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(OSI_MSGQ **)pMsgQ;

    if (NULL == q)
    {
        return IOTK_RES_INVALID_PARAMS;
    }
    //printf("q->queue = %d\n", q->queue);
    err = sem_timedwait(&q->queue, timespec_delay(&delay, Timeout));
    //printf("err = %d, errno = %d\n", err, errno);
    if(-1 == err)
    {
		if (ETIMEDOUT == errno)
		{
			return IOTK_RES_TIMEOUT;
		}
		else{
			return IOTK_RES_FAILURE;
		}
    }

    err = pthread_mutex_lock(&q->guard);

    if (0 != err)
    {
        // this should not be happend!!!
        return IOTK_RES_FAILURE;
    }

    rpointer = (char *)&q[1] + q->rpos * q->msg_size;
    memcpy(pMsg, rpointer, q->msg_size);
    q->rpos = (++q->rpos) % q->max_msgs;
    --q->count;

    pthread_mutex_unlock(&q->guard);

    return IOTK_RES_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// SyncObj
//
typedef struct OSI_SYNCOBJ
{
    sem_t handle;
} OSI_SYNCOBJ;


iotk_res_t iotk_event_create(iotk_event_t* pSyncObj)
{
    int err;
    OSI_SYNCOBJ *obj;
    iotk_res_t res;

    if (pSyncObj == NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    obj = (OSI_SYNCOBJ *)malloc(sizeof(OSI_SYNCOBJ));

    if (NULL == obj)
    {
        return IOTK_RES_MEMORY_ALLOCATION_FAILURE;
    }

    err = sem_init(&obj->handle, 0, 0);

    if (0 != err)
    {
        free(obj);
        return IOTK_RES_OPERATION_FAILED;
    }

    *pSyncObj = obj;
    return IOTK_RES_OK;
}

iotk_res_t iotk_event_delete(iotk_event_t* pSyncObj)
{
    OSI_SYNCOBJ *obj;

    if (pSyncObj == NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    obj = *(OSI_SYNCOBJ **)pSyncObj;

    sem_destroy(&obj->handle);
    free(obj);
    return IOTK_RES_OK;
}

iotk_res_t iotk_event_set(iotk_event_t* pSyncObj)
{
    OSI_SYNCOBJ *obj;

    if (pSyncObj == NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    obj = *(OSI_SYNCOBJ **)pSyncObj;

    return sem_post(&obj->handle) == 0 ? IOTK_RES_OK : IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_event_wait(iotk_event_t* pSyncObj , iotk_time_t Timeout)
{
    int err;
    struct timespec delay;
    OSI_SYNCOBJ *obj;

    if (pSyncObj == NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    obj = *(OSI_SYNCOBJ **)pSyncObj;
    err = sem_timedwait(&obj->handle, timespec_delay(&delay, Timeout));
    if (0 == err)
    {
        return IOTK_RES_OK;
    }
    if (ETIMEDOUT == err)
    {
        return IOTK_RES_TIMEOUT;
    }
    return IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_event_reset(iotk_event_t* pSyncObj)
{
    return iotk_event_wait(pSyncObj, 0);
}


//////////////////////////////////////////////////////////////////////////
//
// Task
//
struct OSI_TASK_PARAMS
{
    void *pValue;
    P_IOTK_TASK_ENTRY pEntry;
};

void *osi_TaskWrapper(void *p)
{
    struct OSI_TASK_PARAMS *params = (struct OSI_TASK_PARAMS *)p;
	if (params->pEntry)
	{
		params->pEntry(params->pValue);
	}
	else{
		printf("pEntry is null");
	}
	
    free(params);
    return NULL;
}

iotk_res_t iotk_task_create(P_IOTK_TASK_ENTRY pEntry, const signed char * const pcName, unsigned short usStackDepth, void *pvParameters, unsigned long uxPriority, iotk_task_t *pTaskHandle)
{
    int err;
    pthread_t hThread;
    //pthread_attr_t attr;
    struct OSI_TASK_PARAMS *params;

#if 0
    err = pthread_attr_init(&attr);
    if (0 != err)
    {
        return IOTK_RES_FAILURE;
    }

    err = pthread_attr_setstacksize(&attr, usStackDepth);
    if (0 != err)
    {
        return IOTK_RES_FAILURE;
    }
#endif

    params = (struct OSI_TASK_PARAMS *)malloc(sizeof(struct OSI_TASK_PARAMS));
    if (NULL == params)
    {
        return IOTK_RES_MEMORY_ALLOCATION_FAILURE;
    }

    params->pEntry = pEntry;
    params->pValue = pvParameters;
    err = pthread_create(&hThread, NULL, osi_TaskWrapper, params);
    if (0 == err)
    {
        pthread_detach(hThread);
        return IOTK_RES_OK;
    }

    return IOTK_RES_FAILURE;
}

void iotk_task_delete(iotk_task_t* pTaskHandle)
{
    // not support
}

void iotk_sleep(unsigned int MilliSecs)
{
    usleep(MilliSecs * 1000);
}

void *mem_Malloc(unsigned long Size)
{
    return malloc(Size);
}

void *mem_Realloc(void *pMem, size_t Size)
{
    return realloc(pMem, Size);
}

void mem_Free(void *pMem)
{
    free(pMem);
}

void mem_set(void *pBuf, int Val, size_t Size)
{
    memset(pBuf, Val, Size);
}

void mem_copy(void *pDst, void *pSrc, size_t Size)
{
    memcpy(pDst, pSrc, Size);
}

#endif // #ifdef WIN32
