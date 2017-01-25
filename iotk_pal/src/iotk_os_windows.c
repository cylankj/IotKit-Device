#if defined(WIN32)
#include <time.h>
#include <windows.h>
#include "iotk/iotk_pal.h"

//////////////////////////////////////////////////////////////////////////
//
// MsgQ
//
typedef struct OSI_MSGQ
{
    HANDLE guard;
    HANDLE queue;
    int rpos;
    int wpos;
    size_t count;
    size_t msg_size;
    size_t max_msgs;
} OSI_MSGQ;


iotk_res_t iotk_msgq_create(iotk_msgq_t *pMsgQ,
                              char *pMsgQName,
                              unsigned long MsgSize,
                              unsigned long MaxMsgs)
{
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

    q->queue = CreateSemaphore(NULL, 0, (LONG)q->max_msgs, NULL);

    if (NULL == q->queue)
    {
        res = IOTK_RES_OPERATION_FAILED;
        goto early_exit;
    }

    q->guard = CreateMutex(NULL, FALSE, NULL);

    if (NULL == q->guard)
    {
        res = IOTK_RES_OPERATION_FAILED;
        goto early_exit;
    }

    res = IOTK_RES_OK;

early_exit:
    if (IOTK_RES_OK != res && q != NULL)
    {
        if (NULL != q->queue)
        {
            CloseHandle(q->queue);
        }

        if (NULL != q->guard)
        {
            CloseHandle(q->guard);
        }

        free(q);
        q = NULL;
    }

    *pMsgQ = q;
    return res;
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

    if (q->queue != NULL)
    {
        CloseHandle(q->queue);
    }

    if (q->guard != NULL)
    {
        CloseHandle(q->guard);
    }

    free(q);

    return IOTK_RES_OK;
}

iotk_res_t iotk_msgq_write(iotk_msgq_t* pMsgQ, void* pMsg , iotk_time_t Timeout)
{
    DWORD err;
    LONG previous = 0;
    char *wpointer = NULL;

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

    err = WaitForSingleObject(q->guard, INFINITE);
    if (WAIT_OBJECT_0 == err)
    {
        if (ReleaseSemaphore(q->queue, 1, &previous))
        {
            wpointer = (char *)&q[1] + q->wpos * q->msg_size;
            memcpy(wpointer, pMsg, q->msg_size);
            q->wpos = (++q->wpos) % q->max_msgs;
            ++q->count;
            res = IOTK_RES_OK;
        }
    }
    ReleaseMutex(q->guard);
    return res;
}

iotk_res_t iotk_msgq_read(iotk_msgq_t* pMsgQ, void* pMsg , iotk_time_t Timeout)
{
    DWORD err;
    char *rpointer = NULL;

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

    err = WaitForSingleObject(q->queue, Timeout);

    if (WAIT_TIMEOUT == err)
    {
        return IOTK_RES_TIMEOUT;
    }
    else if (0 != err)
    {
        return IOTK_RES_FAILURE;
    }

    err = WaitForSingleObject(q->guard, INFINITE);

    if (0 != err)
    {
        // this should not be happend!!!
        return IOTK_RES_FAILURE;
    }

    rpointer = (char *)&q[1] + q->rpos * q->msg_size;
    memcpy(pMsg, rpointer, q->msg_size);
    q->rpos = (++q->rpos) % q->max_msgs;
    --q->count;

    ReleaseMutex(q->guard);

    return IOTK_RES_OK;
}

//////////////////////////////////////////////////////////////////////////
//
// SyncObj
//
typedef struct OSI_SYNCOBJ
{
    HANDLE handle;
} OSI_SYNCOBJ;


iotk_res_t iotk_event_create(iotk_event_t* pSyncObj)
{
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

    obj->handle = CreateSemaphore(NULL, 0, 1, NULL);

    if (INVALID_HANDLE_VALUE == obj->handle || NULL == obj->handle)
    {
        free(obj);
        res = IOTK_RES_OPERATION_FAILED;
    }
    else
    {
        *pSyncObj = obj;
        res = IOTK_RES_OK;
    }

    return res;
}

iotk_res_t iotk_event_delete(iotk_event_t* pSyncObj)
{
    OSI_SYNCOBJ *obj;

    if (pSyncObj == NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    obj = *(OSI_SYNCOBJ **)pSyncObj;

    CloseHandle(obj->handle);

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

    return ReleaseSemaphore(obj->handle, 1, NULL) ? IOTK_RES_OK : IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_event_wait(iotk_event_t* pSyncObj , iotk_time_t Timeout)
{
    DWORD err;
    OSI_SYNCOBJ *obj;

    if (pSyncObj == NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    obj = *(OSI_SYNCOBJ **)pSyncObj;

    err = WaitForSingleObject(obj->handle, Timeout);

    if (WAIT_OBJECT_0 == err)
    {
        return IOTK_RES_OK;
    }
    if (WAIT_TIMEOUT == err)
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

DWORD WINAPI IotkTaskWrapper(void *p)
{
    struct OSI_TASK_PARAMS *params = (struct OSI_TASK_PARAMS *)p;
    params->pEntry(params->pValue);
    free(params);
    return 0;
}

iotk_res_t iotk_task_create(P_IOTK_TASK_ENTRY pEntry, const signed char * const pcName, unsigned short usStackDepth, void *pvParameters, unsigned long uxPriority, iotk_task_t *pTaskHandle)
{
    HANDLE hThread;
    struct OSI_TASK_PARAMS *params;

    params = (struct OSI_TASK_PARAMS *)malloc(sizeof(struct OSI_TASK_PARAMS));

    params->pEntry = pEntry;
    params->pValue = pvParameters;

    hThread = CreateThread(NULL, usStackDepth, IotkTaskWrapper, params, 0, NULL);

    if (NULL != hThread)
    {
        if (pTaskHandle)
        {
            *pTaskHandle = hThread;
        }

        return IOTK_RES_OK;
    }

    return IOTK_RES_FAILURE;
}

void iotk_task_delete(iotk_task_t* pTaskHandle)
{
    HANDLE hThread = (HANDLE)pTaskHandle;
    TerminateThread(hThread, 0);
    CloseHandle(hThread);
}

iotk_res_t iotk_lock_create(iotk_lock_t* pLockObj)
{
    HANDLE hLock;

    if (NULL == pLockObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    hLock = CreateMutex(NULL, FALSE, NULL);

    if (hLock != NULL)
    {  
        *pLockObj = (iotk_lock_t)hLock;
        return IOTK_RES_OK;
    }
    else
    {
        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_lock_delete(iotk_lock_t* pLockObj)
{
    CloseHandle((HANDLE)*pLockObj);
    return IOTK_RES_OK;
}

iotk_res_t iotk_lock_acquire(iotk_lock_t* pLockObj , iotk_time_t Timeout)
{
    if (NULL == pLockObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    if (WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)*pLockObj, Timeout))
    {
        return IOTK_RES_OK;
    }
    else
    {
        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_lock_release(iotk_lock_t* pLockObj)
{
	if (NULL == pLockObj)
	{
		return IOTK_RES_INVALID_PARAMS;
	}

    if (ReleaseMutex((HANDLE)*pLockObj))
    {
    	return IOTK_RES_OK;
    }
    else
    {
    	return IOTK_RES_OPERATION_FAILED;
    }
}

void iotk_sleep(unsigned int MilliSecs)
{
    Sleep(MilliSecs);
}

#endif // #ifdef WIN32
