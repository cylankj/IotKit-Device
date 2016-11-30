#ifndef IOTK_CC3200_FH8610_SPI_MODULE_H
#define IOTK_CC3200_FH8610_SPI_MODULE_H

#define IOTK_SPI_OK                                 (0)
#define IOTK_SPI_ERR_UNINITIALIZE                   (-1)
#define IOTK_SPI_ERR_TIMEDOUT                       (-2)
#define IOTK_SPI_ERR_MALFORM                        (-3)

#define IOTK_CC3200_FH8610_ENTER_BLOCKING           (0)
#define IOTK_CC3200_FH8610_LEAVE_BLOCKING           (1)

#define IOTK_FH8610_POWER_OFF                       (0)
#define IOTK_FH8610_POWER_ON                        (1)
#define IOTK_FH8610_BOOT_UP                         (2)

typedef void (*IOTK_CC3200_FH8610_MODULE_DELEGATE)(int delegate_event);

struct IOTK_CC3200_FH8610_MODULE_CONFIG
{
    IOTK_CC3200_FH8610_MODULE_DELEGATE delegate;

    unsigned use_dma: 1;

    unsigned long peripheral;
    unsigned long bitrate;
    unsigned int  spi_transfer_timeout;

    unsigned char fh8610_power_gpio;
    unsigned char fh8610_power_gpio_pin;
    unsigned int  fh8610_power_gpio_port;

    unsigned char fh8610_reset_gpio;
    unsigned char fh8610_reset_gpio_pin;
    unsigned int  fh8610_reset_gpio_port;

    unsigned int  fh8610_reset_delay;
    unsigned int  fh8610_boot_delay;

    unsigned char fh8610_spi_cs_gpio_init_level;
    unsigned char fh8610_spi_cs_gpio_pin;
    unsigned int  fh8610_spi_cs_gpio_port;
};

int iotk_cc3200_fh8610_spi_open(int handle, int arg1, int mode, int flags);

int iotk_cc3200_fh8610_spi_close(int handle, int arg1, int mode, int flags);

int iotk_cc3200_fh8610_spi_read(int handle, int offset, char *buf, unsigned short len, int flags);

int iotk_cc3200_fh8610_spi_write(int handle, int offset, const char *buf, unsigned short len, int flags);

int iotk_cc3200_fh8610_spi_iotcl(int handle, int code, void *inbuf, unsigned short inlen, const void *outbuf, unsigned short outlen);

int iotk_cc3200_fh8610_power_set(unsigned int power);

int iotk_cc3200_fh8610_power_get(void);

int iotk_cc3200_fh8610_spi_enable(void);

int iotk_cc3200_fh8610_spi_disable(void);

void iotk_cc3200_fh8610_module_task_loop(void);

int iotk_cc3200_fh8610_module_init(struct IOTK_CC3200_FH8610_MODULE_CONFIG *cfg);

#endif // IOTK_CC3200_FH8610_SPI_MODULE_H
