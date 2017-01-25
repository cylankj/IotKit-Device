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

struct OSI_MSGQ
{
    rt_mq_t mq;
    rt_size_t msg_size;
};

iotk_res_t iotk_msgq_create(iotk_msgq_t *pMsgQ, char *pMsgQName, unsigned long MsgSize, unsigned long MaxMsgs)
{
    iotk_res_t res;
    struct OSI_MSGQ *q;

    if (pMsgQ == RT_NULL || MsgSize == 0 || MaxMsgs == 0)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = (struct OSI_MSGQ *)rt_malloc(sizeof(struct OSI_MSGQ));

    if (RT_NULL == q)
    {
        return IOTK_RES_MEMORY_ALLOCATION_FAILURE;
    }

    q->msg_size = MsgSize;
    q->mq = rt_mq_create(pMsgQName ? pMsgQName : "iotk_msgq", MsgSize, MaxMsgs, RT_IPC_FLAG_FIFO);

    if (RT_NULL == q->mq)
    {
        rt_free(q);
        return IOTK_RES_OPERATION_FAILED;
    }

    *pMsgQ = q;
    return IOTK_RES_OK;
}

iotk_res_t iotk_msgq_write(iotk_msgq_t* pMsgQ, void* pMsg, iotk_time_t Timeout)
{
    rt_err_t res;
    struct OSI_MSGQ *q;

    if (RT_NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(struct OSI_MSGQ **)pMsgQ;

    for (;;)
    {
        res = rt_mq_send(q->mq, pMsg, q->msg_size);
        if (res == RT_EOK)
        {
            return IOTK_RES_OK;
        }
        if (res == RT_ETIMEOUT)
        {
            if (Timeout == 0)
            {
                return IOTK_RES_TIMEOUT;
            }
            else
            {
                rt_thread_sleep(rt_tick_from_millisecond(5));
                Timeout -= Timeout > 5 ? 5 : Timeout;
                continue;
            }
        }

        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_msgq_read(iotk_msgq_t* pMsgQ, void* pMsg, iotk_time_t Timeout)
{
    rt_err_t res;
    struct OSI_MSGQ *q;

    if (RT_NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(struct OSI_MSGQ **)pMsgQ;

    if (IOTK_WAIT_FOREVER == Timeout)
    {
        res = rt_mq_recv(q->mq, pMsg, q->msg_size, RT_WAITING_FOREVER);
    }
    else
    {
        res = rt_mq_recv(q->mq, pMsg, q->msg_size, rt_tick_from_millisecond(Timeout));
    }

    if (res == RT_ETIMEOUT) return IOTK_RES_TIMEOUT;
    if (res == RT_EOK) return IOTK_RES_OK;
    return IOTK_RES_OPERATION_FAILED;
}

iotk_res_t iotk_msgq_delete(iotk_msgq_t* pMsgQ)
{
    rt_err_t res;
    struct OSI_MSGQ *q;

    if (RT_NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    q = *(struct OSI_MSGQ **)pMsgQ;
    res = rt_mq_delete(q->mq);

    if (RT_EOK == res)
    {
        rt_free(q);
        return IOTK_RES_OK;
    }

    return IOTK_RES_OPERATION_FAILED;
}

void iotk_sleep(unsigned int MilliSecs)
{
    rt_thread_sleep(rt_tick_from_millisecond(MilliSecs));
}

#endif // #ifdef USE_RTTHREAD
