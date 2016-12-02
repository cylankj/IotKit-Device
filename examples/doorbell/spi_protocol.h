#ifndef __SPI_PROTOCOL__
#define __SPI_PROTOCOL__

#define JFG_SPI_HANDSHAKE_MAGIC             0xC605

// spi action code
#define JFG_SPI_ACT_OPEN                    0x0001
#define JFG_SPI_ACT_READ                    0x0002
#define JFG_SPI_ACT_WRITE                   0x0003
#define JFG_SPI_ACT_IOCTL                   0x0004
#define JFG_SPI_ACT_CLOSE                   0x0005
#define JFG_SPI_ACT_ACK                     0x0006

typedef unsigned short  jfg_spi_action_t;
typedef unsigned short  jfg_spi_length_t;

/*
 *  we should align this struct by 4 bytes
 *  otherwise we will got an ISR error
 */
#pragma pack(push, 4)
struct JFG_SPI_OPEN_CLOSE
{
    unsigned int handle;
    int arg1;
    int mode;
    int flags;
};

struct JFG_SPI_READ_WRITE
{
    unsigned int handle;
    int offset;
    int length;
    int flags;
};

struct JFG_SPI_IOCTL
{
    unsigned int handle;
    int code;
};

struct JFG_SPI_ACK
{
    int code;
};

struct JFG_SPI_HANDSHAKE
{
    unsigned short magic;
    unsigned short padding;
    // 4 bytes
    jfg_spi_action_t action;
    jfg_spi_length_t payload;
    // 8 bytes
    union
    {
        struct JFG_SPI_OPEN_CLOSE open;
        struct JFG_SPI_OPEN_CLOSE close;
        struct JFG_SPI_READ_WRITE read;
        struct JFG_SPI_READ_WRITE write;
        struct JFG_SPI_IOCTL ioctl;
        struct JFG_SPI_ACK ack;
    } op;
};
#pragma pack(pop)

// spi handle value
#define JFG_SPI_HANDLE_DEVICE        (0x80000003)
#define JFG_SPI_HANDLE_RTP           (0x80000004)
#define JFG_SPI_HANDLE_OTA           (0x80000005)
#define JFG_SPI_HANDLE_ANSWER        (0x80000006)
#define JFG_SPI_HANDLE_JPEG          (0x80000007)
#define JFG_SPI_HANDLE_VOICE         (0x80000008)
#define JFG_SPI_HANDLE_RECORD        (0x80000009)
#define JFG_SPI_HANDLE_REPLAY        (0x80000010)
#define JFG_SPI_HANDLE_RECORD_LIST   (0x80000011)
#define JFG_SPI_HANDLE_SDCARD_FORMAT (0x80000012)


#define JFG_AUDIO_FLAG_SPEAKER      0x01
#define JFG_AUDIO_FLAG_MICROPHONE   0x02

// spi ioctl code
#define JFG_SPI_IOCTL_DEV_GETVER    (0x30000001)
#define JFG_SPI_IOCTL_DEV_RING      (0x30000002)
#define JFG_SPI_IOCTL_DEV_SET_TIME  (0x30000003)
#define JFG_SPI_IOCTL_RTP_AUDIO     (0x40000001)
#define JFG_SPI_IOCTL_JPEG_GET_SIZE (0x70000001)

// spi error code
#define JFG_ERROR_NONE              (0)
#define JFG_ERROR_FAILED            (-1)
#define JFG_ERROR_NOT_SUPPORT       (-2)
#define JFG_ERROR_NOT_OPEN          (-2)
#define JFG_ERROR_ALREADY_OPEN      (-3)
#define JFG_ERROR_EAGAIN            (-4)
#define JFG_ERROR_TIMEDOUT          (-5)
#define JFG_ERROR_INVALID_ARGS      (-6)


#endif // __SPI_PROTOCOL__
