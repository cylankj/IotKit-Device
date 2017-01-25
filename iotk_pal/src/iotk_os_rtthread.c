#ifdef USE_RTTHREAD
#include <rtdef.h>
#include <rtthread.h>

#include "iotk/iotk_pal_os.h"

iotk_res_t iotk_event_create(iotk_event_t* pSyncObj)
{
    rt_sem_t sem;

    if (pSyncObj == RT_NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    sem = rt_sem_create("iotk_event", 1, RT_IPC_FLAG_FIFO);

    if (RT_NULL == sem)
    {
        return IOTK_RES_OPERATION_FAILED;
    }

    *pSyncObj = sem;
    return IOTK_RES_OK;
}

iotk_res_t iotk_event_delete(iotk_event_t* pSyncObj)
{
    rt_err_t res;

    if (pSyncObj == RT_NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    res = rt_sem_delete(*(rt_sem_t *)pSyncObj);
    return RT_EOK == res ? IOTK_RES_OK : IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_event_wait(iotk_event_t* pSyncObj , iotk_time_t Timeout)
{
    rt_err_t res;

    if (pSyncObj == RT_NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    if (IOTK_WAIT_FOREVER == Timeout)
    {
        res = rt_sem_take(*(rt_sem_t *)pSyncObj, RT_WAITING_FOREVER);
    }
    else
    {
        res = rt_sem_take(*(rt_sem_t *)pSyncObj, rt_tick_from_millisecond(Timeout));
    }

    if (res == RT_ETIMEOUT) return IOTK_RES_TIMEOUT;
    if (res == RT_EOK) return IOTK_RES_OK;
    return IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_event_set(iotk_event_t* pSyncObj)
{
    rt_err_t res;

    if (pSyncObj == RT_NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    res = rt_sem_release(*(rt_sem_t *)pSyncObj);
    return RT_EOK == res ? IOTK_RES_OK : IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_event_reset(iotk_event_t* pSyncObj)
{
    rt_err_t res;
    res = rt_sem_control(*(rt_sem_t *)pSyncObj, RT_IPC_CMD_RESET, (void *)1);
    return RT_EOK == res ? IOTK_RES_OK : IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_lock_create(iotk_lock_t* pLockObj)
{
    rt_mutex_t mutex;

    if (RT_NULL == pLockObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    mutex = rt_mutex_create("iotk_lock", RT_IPC_FLAG_FIFO);

    if (mutex != RT_NULL)
    {
        *pLockObj = (iotk_lock_t)mutex;
        return IOTK_RES_OK;
    }

    return IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_lock_delete(iotk_lock_t* pLockObj)
{
    rt_err_t res;

    if (pLockObj == RT_NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    res = rt_mutex_delete(*(rt_mutex_t *)pLockObj);
    return RT_EOK == res ? IOTK_RES_OK : IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_lock_acquire(iotk_lock_t* pLockObj , iotk_time_t Timeout)
{
    rt_err_t res;

    if (pLockObj == RT_NULL)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    if (IOTK_WAIT_FOREVER == Timeout)
    {
        res = rt_mutex_take(*(rt_mutex_t *)pLockObj, RT_WAITING_FOREVER);
    }
    else
    {
        res = rt_mutex_take(*(rt_mutex_t *)pLockObj, rt_tick_from_millisecond(Timeout));
    }

    if (res == RT_ETIMEOUT) return IOTK_RES_TIMEOUT;
    if (res == RT_EOK) return IOTK_RES_OK;
    return IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_lock_release(iotk_lock_t* pLockObj)
{
    rt_err_t res;

	if (RT_NULL == pLockObj)
	{
		return IOTK_RES_INVALID_PARAMS;
	}

    res = rt_mutex_release(*(rt_mutex_t *)pLockObj);
    return RT_EOK == res ? IOTK_RES_OK : IOTK_RES_OPERATION_FAILED;
}

#if 0
iotk_res_t iotk_msgq_create(iotk_msgq_t *pMsgQ,
                              char *pMsgQName,
                              unsigned long MsgSize,
                              unsigned long MaxMsgs)
{
    rt_mq_t mq;

	if (RT_NULL == pMsgQ)
	{
		return IOTK_RES_INVALID_PARAMS;
	}

    mq = rt_mq_create(pMsgQName ? pMsgQName : "iotk_mq", MsgSize, MaxMsgs, RT_IPC_FLAG_FIFO);

    if (mq != RT_NULL)
    {
        *pMsgQ = (iotk_msgq_t)mq;
        return IOTK_RES_OK;
    }

    return IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_msgq_delete(iotk_msgq_t* pMsgQ)
{
    OSI_MSGQ *q;

    if (RT_NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(OSI_MSGQ **)pMsgQ;

    if (RT_NULL == q)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    if (q->queue != RT_NULL)
    {
        CloseHandle(q->queue);
    }

    if (q->guard != RT_NULL)
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
    char *wpointer = RT_NULL;

    OSI_MSGQ *q;
    iotk_res_t res = IOTK_RES_FAILURE;

    if (RT_NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(OSI_MSGQ **)pMsgQ;

    if (RT_NULL == q)
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
    char *rpointer = RT_NULL;

    OSI_MSGQ *q;
    iotk_res_t res = IOTK_RES_FAILURE;

    if (RT_NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(OSI_MSGQ **)pMsgQ;

    if (RT_NULL == q)
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
#endif

void iotk_sleep(unsigned int MilliSecs)
{
    rt_thread_sleep(rt_tick_from_millisecond(MilliSecs));
}

#endif // #ifdef USE_RTTHREAD

#if 0
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

#endif // #ifdef USE_RTTHREAD

