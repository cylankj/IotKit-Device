#ifndef __IOTK_PAL_OSI_H__
#define	__IOTK_PAL_OSI_H__

#ifdef CONFIG_IOTK_PAL_USE_OSI

#include <osi.h>

#define IOTK_WAIT_FOREVER                   OSI_WAIT_FOREVER
#define IOTK_NO_WAIT        			    OSI_NO_WAIT

#define IOTK_RES_OK                         OSI_OK
#define IOTK_RES_FAILURE                    OSI_FAILURE
#define IOTK_RES_OPERATION_FAILED           OSI_OPERATION_FAILED
#define IOTK_RES_ABORTED                    OSI_ABORTED
#define IOTK_RES_INVALID_PARAMS             OSI_INVALID_PARAMS
#define IOTK_RES_MEMORY_ALLOCATION_FAILURE  OSI_MEMORY_ALLOCATION_FAILURE
#define IOTK_RES_TIMEOUT                    OSI_TIMEOUT
#define IOTK_RES_EVENTS_IN_USE              OSI_EVENTS_IN_USE
#define IOTK_RES_EVENT_OPEARTION_FAILURE    OSI_EVENT_OPEARTION_FAILURE

#define iotk_res_t                          OsiReturnVal_e
#define iotk_time_t                         OsiTime_t

#define iotk_msgq_t                         OsiMsgQ_t
#define iotk_event_t                        OsiSyncObj_t
#define iotk_lock_t                         OsiLockObj_t
#define iotk_task_t                         OsiSyncObj_t

#define P_IOTK_TASK_ENTRY                   P_OSI_TASK_ENTRY

#define iotk_event_create                   osi_SyncObjCreate
#define iotk_event_delete                   osi_SyncObjDelete
#define iotk_event_set                      osi_SyncObjSignal
#define iotk_event_set_from_isr             osi_SyncObjSignalFromISR
#define iotk_event_wait                     osi_SyncObjWait
#define iotk_event_reset                    osi_SyncObjClear
#define iotk_lock_create                    osi_LockObjCreate
#define iotk_lock_delete                    osi_LockObjDelete
#define iotk_lock_acquire                   osi_LockObjLock
#define iotk_lock_release                   osi_LockObjUnlock
#define iotk_task_create                    osi_TaskCreate
#define iotk_task_delete                    osi_TaskDelete
#define iotk_msgq_create                    osi_MsgQCreate
#define iotk_msgq_delete                    osi_MsgQDelete
#define iotk_msgq_write                     osi_MsgQWrite
#define iotk_msgq_read                      osi_MsgQRead
#define iotk_sleep                          osi_Sleep

#endif // CONFIG_IOTK_PAL_USE_OSI

#endif // __IOTK_PAL_OSI_H__
