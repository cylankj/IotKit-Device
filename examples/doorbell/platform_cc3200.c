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
#include "udma_if.h"
#include "pinmux.h"

#include "logger.h"
#include "common.h"

#include "iotk/iotk_spi_cc3200.h"

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

static void _platform_spi_delegate(int delegate_event)
{
    switch (delegate_event)
    {
    case IOTK_CC3200_FH8610_LEAVE_BLOCKING:
        break;

    case IOTK_CC3200_FH8610_ENTER_BLOCKING:
        break;
    }
}

static void _platform_spi_init(void)
{
    struct IOTK_CC3200_FH8610_MODULE_CONFIG spi_cfg;

    spi_cfg.delegate = _platform_spi_delegate;
    spi_cfg.use_dma = 1;
    spi_cfg.peripheral = PRCM_GSPI;
    spi_cfg.bitrate = CONFIG_SPI_BITRATE;
    spi_cfg.spi_transfer_timeout = CONFIG_SPI_TIMEOUT;
    spi_cfg.fh8610_power_gpio = GPIO_ACC_POWER;
    spi_cfg.fh8610_power_gpio_pin = GPIO_TO_PIN(GPIO_ACC_POWER);
    spi_cfg.fh8610_power_gpio_port = GPIOBase(GPIO_ACC_POWER);
    spi_cfg.fh8610_reset_gpio = GPIO_FH_RESET;
    spi_cfg.fh8610_reset_gpio_pin = GPIO_TO_PIN(GPIO_FH_RESET);
    spi_cfg.fh8610_reset_gpio_port = GPIOBase(GPIO_FH_RESET);
    spi_cfg.fh8610_reset_delay = 50;
    spi_cfg.fh8610_boot_delay = 1100;
    spi_cfg.fh8610_spi_cs_gpio_init_level = GPIO_FH_SPI_INIT_LEVEL;
    spi_cfg.fh8610_spi_cs_gpio_pin = GPIO_TO_PIN(GPIO_FH_SPI);
    spi_cfg.fh8610_spi_cs_gpio_port = GPIOBase(GPIO_FH_SPI);
    iotk_cc3200_fh8610_module_init(&spi_cfg);
    iotk_cc3200_fh8610_spi_enable();
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

    UDMAInit();
    _platform_spi_init();

    //
    // Start the task scheduler
    //
    osi_start();

}

#endif // cc3200
