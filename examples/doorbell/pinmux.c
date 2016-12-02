#ifdef cc3200
#include "pinmux.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "rom.h"
#include "rom_map.h"
#include "gpio.h"
#include "prcm.h"

#include "gpio_if.h"

#include "common.h"
#endif // cc3200

//*****************************************************************************
void PinMuxConfig(void)
{
#ifdef cc3200
    // Enable Peripheral Clocks 
    MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);

    // Configure PIN_55 for UART0 UART0_TX
    MAP_PinTypeUART(PIN_55, PIN_MODE_3);
    // Configure PIN_57 for UART0 UART0_RX
    MAP_PinTypeUART(PIN_57, PIN_MODE_3);

    // SPI
    MAP_PinTypeSPI(PIN_05, PIN_MODE_7);
    MAP_PinTypeSPI(PIN_06, PIN_MODE_7);
    MAP_PinTypeSPI(PIN_07, PIN_MODE_7);
    MAP_PinTypeSPI(PIN_08, PIN_MODE_7);

    // Configure PIN_53 for SPI GPIO Input
    MAP_PinTypeGPIO(GPIO_FH_SPI_PIN, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOBase(GPIO_FH_SPI), GPIO_TO_PIN(GPIO_FH_SPI), GPIO_DIR_MODE_IN);

    // Configure PIN_58 for Reset GPIO Output
    MAP_PinTypeGPIO(GPIO_FH_RESET_PIN, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOBase(GPIO_FH_RESET), GPIO_TO_PIN(GPIO_FH_RESET), GPIO_DIR_MODE_OUT);

    // Configure PIN_63 for Power GPIO Output
    MAP_PinTypeGPIO(GPIO_ACC_POWER_PIN, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOBase(GPIO_ACC_POWER), GPIO_TO_PIN(GPIO_ACC_POWER), GPIO_DIR_MODE_OUT);
#endif // cc3200
}
