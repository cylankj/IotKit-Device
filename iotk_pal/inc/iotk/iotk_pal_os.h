#ifndef __IOTK_PAL_OS_H__
#define	__IOTK_PAL_OS_H__

#ifdef CONFIG_IOTK_PAL_USE_OSI
#include "iotk_pal_osi.h"
#else

#ifdef	__cplusplus
extern "C" {
#endif

#define IOTK_WAIT_FOREVER   			(0xFFFFFFFF)
#define IOTK_NO_WAIT        			(0)

typedef enum
{
  IOTK_RES_OK = 0,
  IOTK_RES_FAILURE = -1,
  IOTK_RES_OPERATION_FAILED = -2,
  IOTK_RES_ABORTED = -3,
  IOTK_RES_INVALID_PARAMS = -4,
  IOTK_RES_MEMORY_ALLOCATION_FAILURE = -5,
  IOTK_RES_TIMEOUT = -6,
  IOTK_RES_EVENTS_IN_USE = -7,
  IOTK_RES_EVENT_OPEARTION_FAILURE = -8
} iotk_res_t;

typedef unsigned int iotk_time_t;

typedef void *iotk_msgq_t;
typedef void *iotk_event_t;
typedef void *iotk_lock_t;
typedef void *iotk_task_t;

typedef void (*P_IOTK_TASK_ENTRY)(void* pValue);


/*!
	\brief 	This function creates a sync object

	The sync object is used for synchronization between different thread or ISR and
	a thread.

	\param	pSyncObj	-	pointer to the sync object control block

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_event_create(iotk_event_t* pSyncObj);


/*!
	\brief 	This function deletes a sync object

	\param	pSyncObj	-	pointer to the sync object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_event_delete(iotk_event_t* pSyncObj);

/*!
	\brief 		This function generates a sync signal for the object.

	All suspended threads waiting on this sync object are resumed

	\param		pSyncObj	-	pointer to the sync object control block

	\return 	upon successful signalling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function could be called from ISR context
	\warning
*/
iotk_res_t iotk_event_set(iotk_event_t* pSyncObj);

/*!
	\brief 		This function generates a sync signal for the object.
				from ISR context.

	All suspended threads waiting on this sync object are resumed

	\param		pSyncObj	-	pointer to the sync object control block

	\return 	upon successful signalling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function is called from ISR context
	\warning
*/
iotk_res_t iotk_event_set_from_isr(iotk_event_t* pSyncObj);

/*!
	\brief 	This function waits for a sync signal of the specific sync object

	\param	pSyncObj	-	pointer to the sync object control block
	\param	Timeout		-	numeric value specifies the maximum number of mSec to
							stay suspended while waiting for the sync signal
							Currently, the simple link driver uses only two values:
								- IOTK_WAIT_FOREVER
								- OSI_NO_WAIT

	\return upon successful reception of the signal within the timeout window return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_event_wait(iotk_event_t* pSyncObj , iotk_time_t Timeout);

/*!
	\brief 	This function clears a sync object

	\param	pSyncObj	-	pointer to the sync object control block

	\return upon successful clearing the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_event_reset(iotk_event_t* pSyncObj);

/*!
	\brief 	This function creates a locking object.

	The locking object is used for protecting a shared resources between different
	threads.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_lock_create(iotk_lock_t* pLockObj);

/*!
	\brief 	This function deletes a locking object.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_lock_delete(iotk_lock_t* pLockObj);

/*!
	\brief 	This function locks a locking object.

	All other threads that call this function before this thread calls
	the iotk_lock_release would be suspended

	\param	pLockObj	-	pointer to the locking object control block
	\param	Timeout		-	numeric value specifies the maximum number of mSec to
							stay suspended while waiting for the locking object
							Currently, the simple link driver uses only two values:
								- IOTK_WAIT_FOREVER
								- OSI_NO_WAIT


	\return upon successful reception of the locking object the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_lock_acquire(iotk_lock_t* pLockObj , iotk_time_t Timeout);

/*!
	\brief 	This function unlock a locking object.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful unlocking the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_lock_release(iotk_lock_t* pLockObj);


/*!
	\brief 	This function call the pEntry callback from a different context

	\param	pEntry		-	pointer to the entry callback function

	\param	pValue		- 	pointer to any type of memory structure that would be
							passed to pEntry callback from the execution thread.

	\param	flags		- 	execution flags - reserved for future usage

	\return upon successful registration of the spawn the function should return 0
			(the function is not blocked till the end of the execution of the function
			and could be returned before the execution is actually completed)
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
/*!
	\brief 	This function creates a Task.

	Creates a new Task and add it to the last of tasks that are ready to run

	\param	pEntry	-	pointer to the Task Function
	\param	pcName	-	Task Name String
	\param	usStackDepth	-	Stack Size Stack Size in 32-bit long words
	\param	pvParameters	-	pointer to structure to be passed to the Task Function
	\param	uxPriority	-	Task Priority

	\return upon successful unlocking the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
iotk_res_t iotk_task_create(P_IOTK_TASK_ENTRY pEntry,const signed char * const pcName,unsigned short usStackDepth,void *pvParameters,unsigned long uxPriority,iotk_task_t *pTaskHandle);

/*!
	\brief 	This function Deletes a Task.

	Deletes a  Task and remove it from list of running task

	\param	pTaskHandle	-	Task Handle

	\note
	\warning
*/
void iotk_task_delete(iotk_task_t* pTaskHandle);


/*******************************************************************************

This function creates a message queue that is typically used for inter thread
communication. 

Parameters:

	pMsgQ		-	pointer to the message queue control block
	pMsgQName	-	pointer to the name of the message queue
	MsgSize		-	the size of the message. 

			NOTICE: THE MESSGAE SIZE MUST BE SMALLER THAN 16

	MaxMsgs		-	maximum number of messages.

Please note that this function allocates the entire memory required 
for the maximum number of messages (MsgSize * MaxMsgs). 

********************************************************************************/
iotk_res_t iotk_msgq_create(iotk_msgq_t* 		pMsgQ , 
							  char*				pMsgQName,
							  unsigned long 		MsgSize,
							  unsigned long 		MaxMsgs);

/*******************************************************************************

This function deletes a specific message queue.
All threads suspended waiting for a message from this queue are resumed with
an error return value. 

Parameters:

	pMsgQ		-	pointer to the message queue control block

********************************************************************************/
iotk_res_t iotk_msgq_delete(iotk_msgq_t* pMsgQ);


/*******************************************************************************

This function writes a message to a specific message queue.

Notice that the message is copied to the queue from the memory area specified 
by pMsg pointer.

--------------------------------------------------------------------------------
THIS FUNCTION COULD BE CALLED FROM ISR AS LONG AS THE TIMEOUT PARAMETER IS 
SET TO "OSI_NO_WAIT"
--------------------------------------------------------------------------------

Parameters:

	pMsgQ		-	pointer to the message queue control block
	pMsg		- 	pointer to the message
	Timeout		-	numeric value specifies the maximum number of mSec to stay 
					suspended while waiting for available space for the message

********************************************************************************/
iotk_res_t iotk_msgq_write(iotk_msgq_t* pMsgQ, void* pMsg , iotk_time_t Timeout);


/*******************************************************************************

This function retrieves a message from the specified message queue. The
retrieved message is copied from the queue into the memory area specified by 
the pMsg pointer 

Parameters:

	pMsgQ		-	pointer to the message queue control block
	pMsg		- 	pointer that specify the location where to copy the message
	Timeout		-	numeric value specifies the maximum number of mSec to stay 
					suspended while waiting for a message to be available

********************************************************************************/
iotk_res_t iotk_msgq_read(iotk_msgq_t* pMsgQ, void* pMsg , iotk_time_t Timeout);

/*!
	\brief 	This function used to suspend the task for the specified number of milli secs
	\param	MilliSecs	-	Time in millisecs to suspend the task
	\return void
	\note
	\warning
*/
void iotk_sleep(unsigned int MilliSecs);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // CONFIG_IOTK_PAL_USE_OSI

#endif // __IOTK_PAL_OS_H__
