#if defined(__UCOS__)
#include "ucos_ii.h"
#include "iotk/iotk_pal_os.h"

#ifndef NULL
#define NULL    ((void *)0)
#endif

//////////////////////////////////////////////////////////////////////////
//
// MsgQ
//
typedef struct OSI_MSGQ
{
    OS_EVENT *queue;
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

    q->queue = OSQCreate((INT8U *)&q[1], MaxMsgs, MsgSize);

    if (NULL == q->queue)
    {
        res = IOTK_RES_OPERATION_FAILED;
        goto early_exit;
    }

    res = IOTK_RES_OK;

early_exit:
    if (IOTK_RES_OK != res && q != NULL)
    {
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

    if (NULL != q->queue)
    {
        OSQDel(q->queue, 0);
    }

    return IOTK_RES_OK;
}

iotk_res_t iotk_msgq_write(iotk_msgq_t* pMsgQ, void* pMsg , iotk_time_t Timeout)
{
    INT8U err;
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

    for (;;)
    {
        err = OSQPost(q->queue, pMsg);
        if (OS_NO_ERR == err)
        {
            return IOTK_RES_OK;
        }
        if (OS_Q_FULL != err)
        {
            return IOTK_RES_FAILURE;
        }
        if (Timeout == IOTK_WAIT_FOREVER)
        {
            iotk_sleep(10);
            continue;
        }
        else if (Timeout > 0)
        {
            iotk_sleep(10);
            Timeout = Timeout > 10 ? Timeout - 10 : 0;
            continue;
        }
        else
        {
            return IOTK_RES_TIMEOUT;
        }
    }
}

iotk_res_t iotk_msgq_read(iotk_msgq_t* pMsgQ, void* pMsg , iotk_time_t Timeout)
{
    INT8U err;
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

    if (0 == Timeout)
    {
        err = OSQAccept(q->queue, (INT8U *)pMsg);
    }
    else if (Timeout == IOTK_WAIT_FOREVER)
    {        
        err = OSQPend(q->queue, 0, (INT8U *)pMsg);
    }
    else
    {
        INT16U ticks;

        Timeout = Timeout < 5 ? 5 : Timeout;
        Timeout = Timeout > 327 ? 327 : Timeout;
        ticks = OS_TICKS_PER_SEC * Timeout / 1000;
        err = OSQPend(q->queue, ticks, (INT8U *)pMsg);
    }

    if (0 == res)
    {
        return IOTK_RES_OK;
    }
    else if (OS_TIMEOUT == res)
    {
        return IOTK_RES_TIMEOUT;
    }

    return IOTK_RES_FAILURE;
}

//////////////////////////////////////////////////////////////////////////
//
// SyncObj
//
typedef struct OSI_SYNCOBJ
{
    void *handle;
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

    obj->handle = OSSemCreate(1);

    if (NULL == obj->handle)
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

    OSQDel(obj->handle, 0);

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

    return OSQPost(obj->handle, obj) == 0 ? IOTK_RES_OK : IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_event_wait(iotk_event_t* pSyncObj , iotk_time_t Timeout)
{
    int storage;
    INT8U err;
    OSI_SYNCOBJ *obj;

    if (pSyncObj == NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    obj = *(OSI_SYNCOBJ **)pSyncObj;

    if (0 == Timeout)
    {
        err = OSQAccept(obj->handle, (INT8U *)&storage);
    }
    else if (Timeout == IOTK_WAIT_FOREVER)
    {        
        err = OSQPend(obj->handle, 0, (INT8U *)&storage);
    }
    else
    {
        INT16U ticks;

        Timeout = Timeout < 5 ? 5 : Timeout;
        Timeout = Timeout > 327 ? 327 : Timeout;
        ticks = OS_TICKS_PER_SEC * Timeout / 1000;
        err = OSQPend(obj->handle, ticks, (INT8U *)&storage);
    }

    if (0 == err)
    {
        return IOTK_RES_OK;
    }
    else if (OS_TIMEOUT == err)
    {
        return IOTK_RES_TIMEOUT;
    }

    return IOTK_RES_FAILURE;
}

iotk_res_t iotk_event_reset(iotk_event_t* pSyncObj)
{
    return iotk_event_wait(pSyncObj, 0);
}


//////////////////////////////////////////////////////////////////////////
//
// Task
//
iotk_res_t iotk_task_create(P_IOTK_TASK_ENTRY pEntry, const signed char * const pcName, unsigned short usStackDepth, void *pvParameters, unsigned long uxPriority, iotk_task_t *pTaskHandle)
{
    // TODO: no implementation
}

void iotk_task_delete(iotk_task_t* pTaskHandle)
{
    // TODO: no implementation
}

void iotk_sleep(unsigned int MilliSecs)
{
    OSTimeDly(((((MilliSecs) * OS_TICKS_PER_SEC) / 1000) > 0) ? (((MilliSecs) * OS_TICKS_PER_SEC) / 1000) : 1);
}

#endif // #ifdef __UCOS__
