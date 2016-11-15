#ifdef USE_FREERTOS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"

#include "iotk/iotk_pal.h"

static portBASE_TYPE _xHigherPriorityTaskWoken = pdFALSE;

iotk_res_t iotk_event_create(iotk_event_t* pSyncObj)
{
    //Check for NULL
    if (NULL == pSyncObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }
    SemaphoreHandle_t *pl_event_ = (SemaphoreHandle_t *)pSyncObj;

    *pl_event_ = xSemaphoreCreateBinary();

    if ((SemaphoreHandle_t)(*pSyncObj) != NULL)
    {
        return IOTK_RES_OK;
    }
    else
    {
        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_event_delete(iotk_event_t* pSyncObj)
{
    //Check for NULL
    if (NULL == pSyncObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }
    vSemaphoreDelete(*pSyncObj);
    return IOTK_RES_OK;
}

iotk_res_t iotk_event_set(iotk_event_t* pSyncObj)
{
    //Check for NULL
    if (NULL == pSyncObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    if (pdTRUE != xSemaphoreGive(*pSyncObj))
    {
        //In case of Semaphore, you are expected to get this if multiple sem
        // give is called before sem take
        return IOTK_RES_OK;
    }

    return IOTK_RES_OK;
}

iotk_res_t iotk_event_wait(iotk_event_t* pSyncObj, iotk_time_t Timeout)
{
    //Check for NULL
    if (NULL == pSyncObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }
    if (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*pSyncObj, (TickType_t)(Timeout / portTICK_PERIOD_MS)))
    {
        return IOTK_RES_OK;
    }
    else
    {
        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_event_reset(iotk_event_t* pSyncObj)
{
    //Check for NULL
    if (NULL == pSyncObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    if (IOTK_RES_OK == iotk_event_wait(pSyncObj, 0))
    {
        return IOTK_RES_OK;
    }
    else
    {
        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_lock_create(iotk_lock_t* pLockObj)
{
    //Check for NULL
    if (NULL == pLockObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }
    *pLockObj = (iotk_lock_t)xSemaphoreCreateMutex();
    if (pLockObj != NULL)
    {
        return IOTK_RES_OK;
    }
    else
    {
        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_task_create(P_IOTK_TASK_ENTRY pEntry, const signed char * const pcName,
    unsigned short usStackDepth, void *pvParameters,
    unsigned long uxPriority, iotk_task_t* pTaskHandle)
{
    if (pdPASS == xTaskCreate(pEntry, (char const*)pcName,
        (usStackDepth / (sizeof(portSTACK_TYPE))),
        pvParameters, (unsigned portBASE_TYPE)uxPriority,
        (TaskHandle_t*)pTaskHandle))
    {
        return IOTK_RES_OK;
    }

    return IOTK_RES_OPERATION_FAILED;
}

void iotk_task_delete(iotk_task_t* pTaskHandle)
{
    vTaskDelete((TaskHandle_t)*pTaskHandle);
}

iotk_res_t iotk_lock_delete(iotk_lock_t* pLockObj)
{
    vSemaphoreDelete((SemaphoreHandle_t)*pLockObj);
    return IOTK_RES_OK;
}

iotk_res_t iotk_lock_acquire(iotk_lock_t* pLockObj, iotk_time_t Timeout)
{
    //Check for NULL
    if (NULL == pLockObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }
    //Take Semaphore
    if (pdTRUE == xSemaphoreTake(*pLockObj, (TickType_t)(Timeout / portTICK_PERIOD_MS)))
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
    //Check for NULL
    if (NULL == pLockObj)
    {
        return IOTK_RES_INVALID_PARAMS;
    }
    //Release Semaphore
    if (pdTRUE == xSemaphoreGive(*pLockObj))
    {
        return IOTK_RES_OK;
    }
    else
    {
        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_msgq_create(iotk_msgq_t *pMsgQ,
    char*			pMsgQName,
    unsigned long 		MsgSize,
    unsigned long		MaxMsgs)
{
    //Check for NULL
    if (NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    QueueHandle_t handle = 0;

    //Create Queue
    handle = xQueueCreate(MaxMsgs, MsgSize);
    if (handle == 0)
    {
        return IOTK_RES_OPERATION_FAILED;
    }

    *pMsgQ = (iotk_msgq_t)handle;
    return IOTK_RES_OK;
}

iotk_res_t iotk_msgq_delete(iotk_msgq_t* pMsgQ)
{
    //Check for NULL
    if (NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }
    vQueueDelete((QueueHandle_t)*pMsgQ);
    return IOTK_RES_OK;
}

iotk_res_t iotk_msgq_write(iotk_msgq_t* pMsgQ, void* pMsg, iotk_time_t Timeout)
{
    //Check for NULL
    if (NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    if (pdPASS == xQueueSendFromISR((QueueHandle_t)*pMsgQ, pMsg, &_xHigherPriorityTaskWoken))
    {
        taskYIELD();
        return IOTK_RES_OK;
    }
    else
    {
        return IOTK_RES_OPERATION_FAILED;
    }
}

iotk_res_t iotk_msgq_read(iotk_msgq_t* pMsgQ, void* pMsg, iotk_time_t Timeout)
{
    //Check for NULL
    if (NULL == pMsgQ)
    {
        return IOTK_RES_INVALID_PARAMS;
    }

    if (Timeout == IOTK_WAIT_FOREVER)
    {
        Timeout = portMAX_DELAY;
    }

    //Receive Item from Queue
    if (pdTRUE == xQueueReceive((QueueHandle_t)*pMsgQ, pMsg, Timeout))
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
	TickType_t xDelay = MilliSecs / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);
}

#if 0
unsigned long iotk_enter_critical(void)
{
    vPortEnterCritical();
    return 0;
}

void iotk_exit_critical(unsigned long ulKey)
{
    vPortExitCritical();
}
#endif

#endif // USE_FREERTOS
