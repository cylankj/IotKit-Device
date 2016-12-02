#include <stdio.h>

#include "iotk/iotk_spi_cc3200.h"
#include "logger/logger.h"
#include "fh8610.h"
#include "spi_protocol.h"

#define CONFIG_SPI_VERIFY_RESULT 1
#if (CONFIG_SPI_VERIFY_RESULT == 1)
#define VERIFY(x)	do { int res = (x); if (IOTK_SPI_ERR_TIMEDOUT == res) LOGLE("SPI TIMEOUT CALL"); return res; } while (0)
#else
#define VERIFY(x)	do { return (x); } while (0)
#endif

int jfg_device_getver(struct JFG_DEVICE_VERSION *ver)
{
    VERIFY(iotk_cc3200_fh8610_spi_iotcl(JFG_SPI_HANDLE_DEVICE, JFG_SPI_IOCTL_DEV_GETVER, ver, sizeof(struct JFG_DEVICE_VERSION), NULL, 0));
}

int jfg_device_ring(void)
{
    VERIFY(iotk_cc3200_fh8610_spi_iotcl(JFG_SPI_HANDLE_DEVICE, JFG_SPI_IOCTL_DEV_RING, NULL, 0, NULL, 0));
}

int jfg_device_set_time(unsigned int time)
{
    VERIFY(iotk_cc3200_fh8610_spi_iotcl(JFG_SPI_HANDLE_DEVICE, JFG_SPI_IOCTL_DEV_SET_TIME, NULL, 0, &time, sizeof(time)));
}

int jfg_rtp_open(unsigned short channel)
{
    VERIFY(iotk_cc3200_fh8610_spi_open(JFG_SPI_HANDLE_RTP, channel, 0, 0));
}

int jfg_rtp_read(char *buf, unsigned short len)
{
    VERIFY(iotk_cc3200_fh8610_spi_read(JFG_SPI_HANDLE_RTP, 0, buf, len, 0));
}

int jfg_rtp_write(const char *buf, unsigned short len)
{
    VERIFY(iotk_cc3200_fh8610_spi_write(JFG_SPI_HANDLE_RTP, 0, buf, len, 0));
}

int jfg_rtp_ioctl_audio(int flags)
{
    VERIFY(iotk_cc3200_fh8610_spi_iotcl(JFG_SPI_HANDLE_RTP, JFG_SPI_IOCTL_RTP_AUDIO, NULL, 0, (const char *)&flags, sizeof(flags)));
}

int jfg_rtp_close()
{
    VERIFY(iotk_cc3200_fh8610_spi_close(JFG_SPI_HANDLE_RTP, 0, 0, 0));
}

int jfg_ota_open()
{
    VERIFY(iotk_cc3200_fh8610_spi_open(JFG_SPI_HANDLE_OTA, 0, 0, 0));
}

int jfg_ota_write(const char *data, unsigned short len)
{
    VERIFY(iotk_cc3200_fh8610_spi_write(JFG_SPI_HANDLE_OTA, 0, data, len, 0));
}

int jfg_ota_close(int mode)
{
    VERIFY(iotk_cc3200_fh8610_spi_close(JFG_SPI_HANDLE_OTA, 0, mode, 0));
}

int jfg_jpeg_open()
{
    VERIFY(iotk_cc3200_fh8610_spi_open(JFG_SPI_HANDLE_JPEG, 0, 0, 0));
}

int jfg_jpeg_get_size()
{
    VERIFY(iotk_cc3200_fh8610_spi_iotcl(JFG_SPI_HANDLE_JPEG, JFG_SPI_IOCTL_JPEG_GET_SIZE, NULL, 0, NULL, 0));
}

int jfg_jpeg_read(char *buf, int len)
{
    VERIFY(iotk_cc3200_fh8610_spi_read(JFG_SPI_HANDLE_JPEG, 0, buf, len, 0));
}

int jfg_jpeg_close()
{
    VERIFY(iotk_cc3200_fh8610_spi_close(JFG_SPI_HANDLE_JPEG, 0, 0, 0));
}
