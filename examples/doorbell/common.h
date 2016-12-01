#ifndef __COMMON__H__
#define __COMMON__H__

#define IOTK_TEST_OLD_LOGIN                 0
#define IOTK_TEST_SERVER                    "yf.jfgou.com"
#define IOTK_TEST_LOG_SERVER                "log.jfgou.com"
#define IOTK_TEST_VID                       "00A8"
#define IOTK_TEST_CID                       "00A8006Bf1ca6007e03751b1e32f287b56fb810a"
#define IOTK_TEST_PID                       107
#define IOTK_TEST_SN                        "67E0BDE3332CAE73"
#define IOTK_TEST_SIG                       "4E93C9264E11FF04A0444D082A70A8585AA175D516FF1C5EB1858B16849C168B116AD51FA149B0DF1434620F135167BF53B43D537690BE966D79E8DD0B01D6F6"

#define IOTK_TEST_CID_OLD                   "200000000001"

#define SECURITY_TYPE                       SL_SEC_TYPE_OPEN

#define CONFIG_SPI_BITRATE                  3375000
#define CONFIG_SPI_TIMEOUT                  1000


#define GPIO_ACC_POWER                      8
#define GPIO_ACC_POWER_PIN                  PIN_63

#define GPIO_FH_RESET                       12
#define GPIO_FH_RESET_PIN                   PIN_03

#define GPIO_FH_SPI                         30
#define GPIO_FH_SPI_PIN                     PIN_53
#define GPIO_FH_SPI_INIT_LEVEL              0

#define SSID_NAME               "iotk_demo"
#define SECURITY_KEY            ""

#define CONFIG_CFG_FILE_SIZE_ALIGN          1024

#ifndef HALT
#define HALT()            		for (;;)
#endif

#ifndef ASSERT
#define ASSERT(x) \
    do { \
    if (!(x)) { \
        LOGF("ASSERTION FAILED: " #x); \
        HALT(); \
    } \
    } while (0)
#endif

#define SPAWN_TASK_PRIORITY     9
#define MAIN_TASK_STACK_SIZE    2048
#define MAIN_TASK_PRIORITY      1
#define SL_STOP_TIMEOUT         200
#define UNUSED(x)               ((x) = (x))
#define SUCCESS                 0
#define FAILURE                 -1


#ifndef FLAG_SET
#define FLAG_SET(f, s)                  ((s) |= (f))
#endif

#ifndef FLAG_CLR
#define FLAG_CLR(f, s)                  ((s) &= ~(f))
#endif

#ifndef FLAG_ISSET
#define FLAG_ISSET(f, s)                ((f) == ((f) & (s)))
#endif

#ifndef MIN
#define MIN(a, b)                       ((a) > (b) ? (b) : (a))
#endif

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(x)                 (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ALIGN_N
#define ALIGN_N(x, n)                   (((long)x + ((n) - 1)) & ~((n) - 1))
#endif

#ifndef SPLIT_IP
#define SPLIT_IP(x)                     (x >> 24), ((x >> 16) & 0xFF), ((x >> 8) & 0xFF), (x & 0xFF)
#endif

#ifndef GPIO_TO_PIN
#define GPIO_TO_PIN(x)                  (1 << ((x) % 8))
#endif

extern void platform_init(void);
extern void application_main(void *p);

#endif // __COMMON__H__

