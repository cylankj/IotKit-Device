#ifndef __FH8610_H__
#define __FH8610_H__

#define JFG_OTA_MODE_ABORT          (0)
#define JFG_OTA_MODE_COMMIT         (1)

#pragma pack(push, 4)
struct JFG_DEVICE_VERSION
{
    unsigned int hardware_id;
    unsigned int software_version;
};
#pragma pack(pop)

int jfg_device_getver(struct JFG_DEVICE_VERSION *ver);

int jfg_device_ring(void);

int jfg_device_set_time(unsigned int time);

int jfg_rtp_open(unsigned short channel);

int jfg_rtp_read(char *buf, unsigned short len);

int jfg_rtp_write(const char *buf, unsigned short len);

int jfg_rtp_ioctl_audio(int flags);

int jfg_rtp_close();

int jfg_ota_open();

int jfg_ota_write(const char *data, unsigned short len);

int jfg_ota_close(int mode);

int jfg_jpeg_open();

int jfg_jpeg_get_size();

int jfg_jpeg_read(char *buf, int len);

int jfg_jpeg_close();

#endif // __FH8610_H__

