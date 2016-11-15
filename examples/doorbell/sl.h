#ifndef SL_H
#define SL_H

#define SL_ROLE_STOP                    (-1)
#define SL_ROLE_STA                     (0)
#define SL_ROLE_AP                      (2)

#define SL_SSID_LEN_MAX                 32
#define SL_BSSID_LEN_MAX                6

#define SL_WLAN_MAC_ADDR_LEN            18
#define SL_WLAN_INVALID_ADDRESS         (0xFFFFFFFF)
#define SL_WLAN_PROFILE_REMOVE_ALL      (0xFF)

enum SL_EVENT_ID
{
    SL_EVENT_STA_CONNECTED = 1,
    SL_EVENT_STA_DISCONNECTED,
    SL_EVENT_STA_IP_LEASED,
    SL_EVENT_STA_IP_RELEASED,
    SL_EVENT_WLAN_CONNECTED,
    SL_EVENT_WLAN_CONNECT_FAILED,
    SL_EVENT_WLAN_USER_INITIATED_DISCONNECT,
    SL_EVENT_WLAN_DISCONNECT,
    SL_EVENT_WLAN_IP_ACQUIRED,
    SL_EVENT_SOCKET_EXCEPTION,
    SL_EVENT_GENERAL_EVENT,
};

struct SL_EVENT_MSG
{
    enum SL_EVENT_ID id;

    union
    {
        struct
        {
            unsigned int ip_be;
        } sta_ip;

        struct
        {
            unsigned int ip_be;
            unsigned int gw_be;
            unsigned int dns_be;
        } wlan_ip;

        struct
        {
            char bssid[SL_BSSID_LEN_MAX];
        } wlan_connected;

        struct
        {
            unsigned int event;
            union
            {
                struct
                {
                    int type;
                    int data;
                } abort;
                struct
                {
                    unsigned char status;
                    int sender;
                } general;
                int info;
            } u;
        } general_event;

        struct
        {
            unsigned int event;
            short status;
            unsigned char fd;
            unsigned char padding;
        } socket_exception;
    } u;

};

struct SL_PING_REPORT
{
    unsigned int pkts_sent;
    unsigned int pkts_received;
    unsigned short min_round_time;
    unsigned short max_round_time;
    unsigned short avg_round_time;
    unsigned int test_time;
};

typedef void (*SL_PING_CALLBACK)(struct SL_PING_REPORT *);

typedef void (*SL_EVENT_DELEGATE)(struct SL_EVENT_MSG *);

void sl_set_delegate(SL_EVENT_DELEGATE delegate);

void sl_print_version(void);

void sl_init(void);

int sl_start(void);

int sl_stop(void);

int sl_reset(void);

int sl_wlan_disconnect(void);

int sl_wlan_is_connected(void);

int sl_wlan_profile_remove(const short idx);

unsigned char sl_wlan_profile_remove_set(unsigned char set);

int sl_wlan_profile_count(unsigned char *profile_set);

int sl_wlan_profile_add(const char *ssid, const char *psk);

int sl_wlan_profile_get(char *ssidout, int size);

int sl_wlan_get_ap_ssid(char *ssid, unsigned short len);

int sl_wlan_initialize_ap(const char *ssid, unsigned char security, const char *psk);

int sl_wlan_initialize_sta(void);

int sl_wlan_role_set(int mode);

int sl_wlan_role_get(void);

int sl_wlan_get_ssid(char ssid[32]);

int sl_wlan_get_mac(unsigned char mac[6]);

int sl_wlan_get_bssid(unsigned char bssid[6]);

unsigned int sl_wlan_get_ip(void);

unsigned int sl_wlan_get_gateway(void);

unsigned int sl_wlan_get_dns(void);

int sl_file_load(const char *path, void *buf, int size);

int sl_file_save(const char *path, const void *buf, int size);

int sl_file_length(const char *path);

void sl_wlan_policy_set_long_sleep(int interval);

int sl_wlan_network_scan(int enable, int interval_sec);

int sl_wlan_rx_stats(void);

void sl_wlan_dump_hotspots();

void sl_netcfg_update_ip(void);

int sl_netapp_ping(unsigned int ip_be, unsigned int interval_ms,
        unsigned char pkt_count, unsigned short pkt_size,
        unsigned short timeout_ms, SL_PING_CALLBACK callback, struct SL_PING_REPORT *report);

#endif // SL_H
