#ifndef __IOTK_API_H__
#define __IOTK_API_H__

#define IOTK_SN_LENGTH                  40
#define IOTK_CID_LENGTH                 40
#define IOTK_COMPAT_CID_LENGTH          12
#define IOTK_SIGNATURE_LENGTH           128

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
    IOTK_EVENT_TIMER_TIMEOUT,
    IOTK_EVENT_ROLINK_CONNECTED,
    IOTK_EVENT_ROLINK_DISCONNECTED,
    IOTK_EVENT_ROLINK_REGISTER_ACK,
    IOTK_EVENT_ROLINK_LOGIN_ACK,
    IOTK_EVENT_ROLINK_BIND_CID_ACK,
    IOTK_EVENT_ROLINK_GET_DP_ACK,
    IOTK_EVENT_ROLINK_SET_DP_ACK,
    IOTK_EVENT_ROLINK_DEL_DP_ACK
};


enum IOTK_DPID
{
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

struct IOTK_EVENT
{
    enum IOTK_EVENT_ID id;

    union
    {
        struct
        {
            int err_code;
        } disconnect, logged_in, bind_cid_ack;

        struct  
        {
            int timer_id;
        } timer_timeout;

        struct
        {
            int err_code;
            const char *cid;
            unsigned int cid_length;
        } cid_register;

        struct
        {
            unsigned long long seq;
            int dpid;
            unsigned long long ts;
            int err_code;
        } set_dp_ack;

        struct
        {
            unsigned long long seq;
            int dpid;
            unsigned int index;
            unsigned int total;
            unsigned long long ts;
            const unsigned char *data;
            unsigned int length;
        } get_dp_ack;
    } u;
};

struct IOTK_DP_KEY
{
    int dpid;
    unsigned long long ts;
};

struct IOTK_DP_VALUE
{
    unsigned char *data;
    unsigned int length;
};

struct IOTK_DP
{
    struct IOTK_DP_KEY key;
    struct IOTK_DP_VALUE value;
};

struct timeval;

typedef void (*IOTK_EVENT_CALLBACK)(const struct IOTK_EVENT *e);

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

int iotk_run(void);

/** @fn int iotk_cfg_set(int cfg, int len, ...)
 *  @brief set IoTKit configure
 *  @param cfg [in] config id
 *  @param len [in] config value length
 *  @param ... [in] config value
 */
int iotk_cfg_set(int cfg, int len, ...);

int iotk_cfg_get(int cfg, int len, void *val);

int iotk_start(void);

int iotk_stop(void);

int iotk_timer_new(unsigned int expiration);

int iotk_timer_set_expiration(int timer_id, unsigned int expiration);

int iotk_timer_free(int timer_id);

int iotk_rolink_set_dp(unsigned long long seq, const struct IOTK_DP *dps, unsigned int count);

int iotk_rolink_get_dp(unsigned long long seq, unsigned int limit, int ascent, 
                         const struct IOTK_DP_KEY *keys, unsigned int count);

int iotk_rolink_del_dp(unsigned long long seq, const struct IOTK_DP_KEY *keys, unsigned int count);

int iotk_rolink_count_dp(unsigned long long seq, int act, int *dpid_list, unsigned int count);

int iotk_rolink_login(unsigned int pid, const char *sn, const char *signature, const char *cid);

int iotk_rolink_compat_login(unsigned int pid, const char *cid, int net, 
                             unsigned int uptime, const char *ssid,
                             const char *softver, const char *sysver, const char *mac);

int iotk_rolink_register_cid(const char *vid, unsigned int pid, const char *sn, const char *signature);

int iotk_rolink_bind_cid(const char *bind_code);


extern unsigned int iotk_time_tick(void);

#ifdef __cplusplus
};
#endif

#endif // __IOTK_API_H__
