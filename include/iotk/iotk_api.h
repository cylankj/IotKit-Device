#ifndef __IOTK_API_H__
#define __IOTK_API_H__

#define IOTK_SN_LENGTH                  40
#define IOTK_CID_LENGTH                 40
#define IOTK_COMPAT_CID_LENGTH          12
#define IOTK_SIGNATURE_LENGTH           128

/** @struct IOTK_GUID
 *  @brief iotk sdk guid struct
 */
struct IOTK_GUID
{
    unsigned long  data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char  data4[8];
};

/** @struct IOTK_VERSION
 *  @brief iotk sdk version struct
 */
struct IOTK_VERSION
{
    unsigned char major;
    unsigned char minor;
    unsigned char patch;
    unsigned char tweak;
};

/** @struct IOTK_NETWORK_CONFIG
 *  @brief iotk network config struct
 */
struct IOTK_NETWORK_CONFIG
{
    unsigned int ip;                    /**< local ipv4 address in network byte order */
    unsigned int gateway;               /**< gateway ipv4 address in network byte order */
    unsigned int dns;                   /**< dns ipv4 address in network byte order */
};

/** @enum IOTK_CFG_ID
 *  @brief iotk configure id 
 */
enum IOTK_CFG_ID
{
    IOTK_CFG_NETWORK,                   /**< network config */
    IOTK_CFG_SERVER_ADDR,               /**< rolink server address */
    IOTK_CFG_SERVER_PORT,               /**< rolink server port */
    IOTK_CFG_LISTEN_PORT,               /**< rolink local service port */
    IOTK_CFG_DISCOVERY_PORT,            /**< rolink local discovery service port */
    IOTK_CFG_EVENT_CALLBACK,            /**< iotk event callback function */
};

/** @enum IOTK_EVENT_ID
 *  @brief iotk callback event id 
 */
enum IOTK_EVENT_ID
{
    IOTK_EVENT_ENTER_BLOCKING,
    IOTK_EVENT_LEAVE_BLOCKING,
    IOTK_EVENT_CONNECTED,
    IOTK_EVENT_DISCONNECTED,
    IOTK_EVENT_REGISTER_ACK,
    IOTK_EVENT_LOGIN_ACK,
    IOTK_EVENT_BIND_CID_ACK,
    IOTK_EVENT_GET_DP_REQ,
    IOTK_EVENT_GET_DP_ACK,
    IOTK_EVENT_SET_DP_REQ,
    IOTK_EVENT_SET_DP_ACK,
    IOTK_EVENT_DEL_DP_ACK,
    IOTK_EVENT_P2P_SETUP,
    IOTK_EVENT_P2P_SDP,
    IOTK_EVENT_P2P_DISCONNECT,
};

/** @enum IOTK_DPID
 *  @brief iotk datapoint id list
 */
enum IOTK_DPID
{
    IOTK_DPID_DEVICE_MODEL = 100,
    IOTK_DPID_DEVICE_WIFI,
    IOTK_DPID_DEVICE_SERVER,
    IOTK_DPID_DEVICE_LOG_SERVER,

    IOTK_DPID_BASE_BEGIN = 200,
    IOTK_DPID_BASE_NET,
    IOTK_DPID_BASE_MAC,
    IOTK_DPID_BASE_SDCARD_PRESENT,
    IOTK_DPID_BASE_SDCARD_CAPACITY,
    IOTK_DPID_BASE_CHARGE,
    IOTK_DPID_BASE_POWER,
    IOTK_DPID_BASE_BATTERY,
    IOTK_DPID_BASE_SW_VERSION,
    IOTK_DPID_BASE_SYS_VERSION,
    IOTK_DPID_BASE_LED,
    IOTK_DPID_BASE_UPTIME,
    IOTK_DPID_BASE_CLIENT_LOG,
    IOTK_DPID_BASE_DEVICE_LOG,
    IOTK_DPID_BASE_P2P_VERSION,
    IOTK_DPID_BASE_TIMEZONE,
    IOTK_DPID_BASE_IS_PUSH_FLOW,
    IOTK_DPID_BASE_IS_NTSC,
    IOTK_DPID_BASE_IS_MOBILE,
    IOTK_DPID_BASE_FORMAT_SD,
    IOTK_DPID_BASE_BIND,

    IOTK_DPID_BELL_BEGIN = 400,
    IOTK_DPID_BELL_CALL,
    IOTK_DPID_BELL_LEAVE_MSG,
    IOTK_DPID_CAMERA_BEGIN = 500,
    IOTK_DPID_CAMERA_WARN_SETTINGS,
    IOTK_DPID_CAMERA_WARN_TIME,
    IOTK_DPID_CAMERA_WARN_SENSITIVITY,
    IOTK_DPID_CAMERA_WARN_SOUND,
    IOTK_DPID_CAMERA_WARN_REPORT,
};

struct P2P_SDP;
struct P2P_PEER;
struct IRSTREAM;
struct IWSTREAM;
struct timeval;
struct IOTK_IUNKNOWN;

/** @struct IOTK_EVENT
 *  @brief iotk run loop event
 */
struct IOTK_EVENT
{
    enum IOTK_EVENT_ID id;

    union
    {
        struct
        {
            unsigned long long seq;
            int err_code;
        } disconnect, logged_in, bind_cid_ack;

        struct
        {
            unsigned long long seq;
            int err_code;
            const char *cid;
            unsigned int cid_length;
        } cid_register;

        struct
        {
            unsigned long long seq;
            unsigned int limit;
            int ascent;
            unsigned int count;
            struct IRSTREAM *irs;
        } get_dp_req;

        struct
        {
            unsigned long long seq;
            unsigned int count;
            struct IRSTREAM *irs;
        } get_dp_ack, set_dp_ack;

        struct
        {
            const char *caller;
            unsigned long long seq;
            unsigned int call_id;
            unsigned int channel;
            struct P2P_PEER *peer;
        } p2p_setup;

        struct 
        {
            const char *caller;
            unsigned long long seq;
            unsigned int call_id;
            struct P2P_SDP *sdp;
        } p2p_sdp;
    } u;
};

typedef void (*IOTK_EVENT_CALLBACK)(void *s, struct IOTK_EVENT *e);

typedef void *(*IOTK_QUERY_INTERFACE)(struct IOTK_IUNKNOWN *, const struct IOTK_GUID *);

struct IOTK_IUNKNOWN
{
    IOTK_QUERY_INTERFACE query_interface;
};

typedef void (*IOTK_MSG_WRITER)(struct IWSTREAM *s, void *ctx);

struct IOTK_INTERFACE_DEVICE
{
    struct IOTK_IUNKNOWN iunknown;
    int (*login)(struct IOTK_IUNKNOWN *i, unsigned long long seq, unsigned int pid, const char *sn, const char *signature, const char *cid);
    int (*cid_register)(struct IOTK_IUNKNOWN *i, unsigned long long seq, const char *vid, unsigned int pid, const char *sn, const char *signature);
    int (*cid_bind)(struct IOTK_IUNKNOWN *i, unsigned long long seq, const char *bind_code);
    int (*p2p_setup_ack)(struct IOTK_IUNKNOWN *i, unsigned long long seq, const char *callee);
    int (*p2p_sdp_update)(struct IOTK_IUNKNOWN *i, unsigned long long seq, const char *callee, unsigned int call_id, struct P2P_SDP *sdp);
    int (*p2p_disconnect)(struct IOTK_IUNKNOWN *i, unsigned long long seq, const char *callee, unsigned int call_id, int reason);
    int (*get_dp)(struct IOTK_IUNKNOWN *i, unsigned long long seq, unsigned int limit, int ascent, unsigned int count, IOTK_MSG_WRITER writer, void *ctx);
    int (*set_dp)(struct IOTK_IUNKNOWN *i, unsigned long long seq, unsigned int count, IOTK_MSG_WRITER writer, void *ctx);
};

// {996581AD-168A-4c60-8AB2-DA63143A02A0}
static const struct IOTK_GUID iotk_iid_device = { 0x996581ad, 0x168a, 0x4c60, { 0x8a, 0xb2, 0xda, 0x63, 0x14, 0x3a, 0x2, 0xa0 } };

#ifdef __cplusplus
extern "C"
{
#endif

/** @fn int iotk_init(void)
 *  @brief initialize IoTKit runtime environment
 */
int iotk_init(void);

/** @fn int iotk_version_get(struct IOTK_VERSION *ver)
 *  @brief get the version of IoTKit sdk
 *  @param ver [out] return version of iotk sdk
 */
int iotk_version_get(struct IOTK_VERSION *ver);

/** @fn int iotk_cfg_set(int cfg, int len, ...)
 *  @brief set IoTKit configure
 *  @param cfg [in] config id
 *  @param len [in] config value length
 *  @param ... [in] config value
 *  @return error code
 *  @retval 0 success
 *  @retval -1 failed
 */
int iotk_cfg_set(int cfg, int len, ...);

/** @fn int iotk_cfg_get(int cfg, int len, void *val)
 *  @brief get IoTKit configure
 *  @param cfg [in] config id
 *  @param len [in] config value buffer size
 *  @param val [out] config value
 *  @return error code
 *  @retval 0 success
 *  @retval -1 failed
 */
int iotk_cfg_get(int cfg, int len, void *val);

int iotk_start(void);

int iotk_stop(void);

int iotk_run(int timeout_ms);

typedef void (*IOTK_SPAWN_ENTRY)(void *);
int iotk_spawn(IOTK_SPAWN_ENTRY entry, void *arg);

int iotk_wakeup(void);

struct IOTK_IUNKNOWN *iotk_query_interface(const struct IOTK_GUID *iid);

#ifdef __cplusplus
};
#endif

#endif // __IOTK_API_H__
