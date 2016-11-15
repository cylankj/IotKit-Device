#ifdef cc3200

// Simplelink includes
#include "simplelink.h"

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "uart.h"
#include "prcm.h"
#include "utils.h"

// Free_rtos/ti-rtos includes
#include "osi.h"

// Common interface includes
#include "gpio_if.h"
#include "uart_if.h"
#include "pinmux.h"

#include "logger.h"
#include "common.h"

#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

#ifdef USE_FREERTOS
void vAssertCalled( const char *pcFile, unsigned long ulLine )
{
    //Handle Assert here
    while(1)
    {
    }
}

void vApplicationIdleHook( void)
{
    //Handle Idle Hook for Profiling, Power Management etc
}

void vApplicationMallocFailedHook()
{
    //Handle Memory Allocation Errors
    while(1)
    {
    }
}

void vApplicationStackOverflowHook(OsiTaskHandle *pxTask, signed char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
#endif // USE_FREERTOS

static void BoardInit(void)
{
// In case of TI-RTOS vector table is initialize by OS itself
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif //USE_TIRTOS

    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

void *logger_lock_create()
{
    OsiLockObj_t lock;
    osi_LockObjCreate(&lock);
    return lock;
}

void logger_lock_acquire(void *lock)
{
    osi_LockObjLock(&lock, OSI_WAIT_FOREVER);
}

void logger_lock_release(void *lock)
{
    osi_LockObjUnlock(&lock);
}

static void logger_writer(struct LOGGER_APPENDER *appender, int level, const char *buf, int len)
{
    const char *p = buf;

    if (p != NULL)
    {
        while(*p != '\0')
        {
            MAP_UARTCharPut(CONSOLE, *p++);
        }
    }
}

void platform_init(void)
{
	long lRetVal;

    //
    // Board Initialization
    //
    BoardInit();

    //
    // configure the GPIO pins for LEDs,UART
    //
    PinMuxConfig();

    //
    // Configure the UART
    //
#ifndef NOTERM
    InitTerm();

    ClearTerm();
#endif  //NOTERM

    my_logger_init(logger_writer);

    //
    // Start the SimpleLink Host
    //
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    ASSERT(lRetVal >= 0);

    //
    // Start the IoTKit runloop task
    //
    lRetVal = osi_TaskCreate(application_main, (const signed char*)"appmain",
    						MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, NULL);
    ASSERT(lRetVal >= 0);

    //
    // Start the task scheduler
    //
    osi_start();

}

#endif // cc3200
