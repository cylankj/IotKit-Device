#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sl.h"

#ifdef cc3200
#include <osi.h>
#include <fs.h>
#include <simplelink.h>
#endif // cc3200

#include "logger/logger.h"
#include "common.h"

#define SL_WLAN_SCAN_MAX_ENTRIES        		(20)
#define SL_WLAN_MAX_PROFILES			        (7)

#define BELL_WIFI_BSSID_LEN_MAX       			(6)

#if (CONFIG_WIFI_DEBUG == 1)
#define SL_DEBUG                                LOGLD
#define SL_ERROR                                LOGLE
#else
#define SL_DEBUG(...)
#define SL_ERROR(...)
#endif

#ifndef SPLIT_IP
#define SPLIT_IP(x)                             (x >> 24), ((x >> 16) & 0xFF), ((x >> 8) & 0xFF), (x & 0xFF)
#endif

#define SL_FIRE_EVENT(s, e)	                    do { if ((s)->delegate) { (s)->delegate(e); } } while (0)

struct SL_STATE
{
    int role;
    unsigned int connected:1, reserved:31;
    unsigned long device_ip;
    unsigned long gateway_ip;
    unsigned long dns_ip;
    unsigned long ap_client_ip;
    SL_PING_CALLBACK ping_callback;
    struct SL_PING_REPORT *ping_report;
    char ssid[SL_SSID_LEN_MAX + 1];
    char bssid[SL_BSSID_LEN_MAX];
    SL_EVENT_DELEGATE delegate;
};

// typedef void (*CC3200_WIFI_SCAN_CALLBACK)(void *, Sl_WlanNetworkEntry_t *);

static struct SL_STATE g_sl;

static struct SL_STATE *sl_get()
{
    return &g_sl;
}

void sl_set_delegate(SL_EVENT_DELEGATE delegate)
{
#ifdef cc3200
    sl_get()->delegate = delegate;
#endif
}

void sl_print_version(void)
{
#ifdef cc3200
    SlVersionFull ver;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;

    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    memset(&ver, 0, sizeof(SlVersionFull));
    sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt,
                                &ucConfigLen, (unsigned char *)(&ver));

    LOGLI("sl: Host Driver Version: %s", SL_DRIVER_VERSION);
    LOGLI("sl: Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d" ,
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);
#endif
}

int sl_start(void)
{
#ifdef cc3200
    int res;
    struct SL_STATE *w = sl_get();

    w->device_ip = SL_WLAN_INVALID_ADDRESS;
    w->gateway_ip = SL_WLAN_INVALID_ADDRESS;

    res = sl_Start(NULL, NULL, NULL);
    //LOGLV("sl_Start() = %d" , res);
    w->role = res;

#if 0
    if (SL_ROLE_STA == res)
    {
        res = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
        LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
            "sl_WlanPolicySet(autoconnect) = %d", res);
    }
#endif

    return w->role;
#else
    return 2;
#endif
}

int sl_stop(void)
{
#ifdef cc3200
    int res;
    //int retry;
    struct SL_STATE *w = sl_get();

    // make sure simplelink was started
    if (SL_ROLE_STOP == w->role)
    {
        LOGLW("sl_stop(): not start yet");
        return 0;
    }

    res = sl_Stop(0xFF);
    w->role = -1;
    LOGL(res == 0 ? LOGGER_LEVEL_INFO : LOGGER_LEVEL_VERBOSE,
         "sl_Stop(0xFF) = %d" , res);
    return res;
#else
    return 0;
#endif
}

int sl_reset(void)
{
#ifdef cc3200
    int res;
    unsigned char optu8;
    _u8 country_code[] = "EU";
    _WlanRxFilterOperationCommandBuff_t RxFilterIdMask;
    struct SL_STATE *w = sl_get();

    if (ROLE_AP == w->role)
    {
#if 0
        while (w->device_ip == SL_WLAN_INVALID_ADDRESS)
        {
            SL_DEBUG("Waitting for ip acquire..." );
            osi_Sleep(100);
        }
#endif
    }
    else if (ROLE_STA == w->role)
    {
        res = sl_WlanDisconnect();
        LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
            "sl_WlanDisconnect() = %d", res);
#if 0
        if (0 == res)
        {
            // Wait
            while (SL_WLAN_INVALID_ADDRESS != w->device_ip)
            {
                SL_DEBUG("sl: waitting for wlan disconnect..., current IP=%08X" , w->device_ip);
                osi_Sleep(100);
            }
        }
#endif
    }
    else
    {
        LOGLE("sl: unexpected role: %d, reset failed", w->role);
        return -1;
    }

    res = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_WlanPolicySet(SL_CONNECTION_POLICY) = %d", res);

#if 0
    // Disable DHCP client
    optu8 = 0;
    res = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, 1, &optu8);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE=0) = %d", res);

    SlNetCfgIpV4Args_t ipV4;
    ipV4.ipV4          = (_u32)SL_IPV4_VAL(192,168,137,10);        // _u32 IP address
    ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);         // _u32 Subnet mask for this STA/P2P
    ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(192,168,137,1);         // _u32 Default gateway address
    ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(192,168,137,1);         // _u32 DNS server address

    res = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, sizeof(SlNetCfgIpV4Args_t),(_u8 *)&ipV4);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE) = %d", res);
#else
    // Enable DHCP client
    optu8 = 1;
    res = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, 1, &optu8);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE=1) = %d", res);

    // Set AP mode static ip
    SlNetCfgIpV4Args_t ipV4;
    ipV4.ipV4          = (_u32)SL_IPV4_VAL(192,168,1,1);        // _u32 IP address
    ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);      // _u32 Subnet mask
    ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(192,168,1,1);        // _u32 Default gateway address
    ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(192,168,1,1);        // _u32 DNS server address

    optu8 = 1;
    res = sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4,
            sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE) = %d", res);
#endif

    // disable http server
    res = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID) = %d", res);

    // disable mdns
    res = sl_NetAppStop(SL_NET_APP_MDNS_ID);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_NetAppStop(SL_NET_APP_MDNS_ID) = %d", res);

    // Set country code // Both EU and JP supports channels 12 and 13 (US / JP /EU)
    res = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_COUNTRY_CODE, 2, country_code);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_WlanSet(Country(EU)) = %d", res);

    // Disable scan
    res = sl_WlanPolicySet(SL_POLICY_SCAN, SL_SCAN_POLICY(0), NULL, 0);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_WlanPolicySet(SL_SCAN_POLICY(0)) = %d", res);

    res = sl_WlanPolicySet(SL_POLICY_PM, SL_ALWAYS_ON_POLICY, NULL, 0);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_WlanPolicySet(SL_ALWAYS_ON_POLICY) = %d", res);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    optu8 = 0;
    res = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&optu8);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_WlanSet(WLAN_GENERAL_PARAM_OPT_STA_TX_POWER) = %d", res);

    // Set PM policy to normal
    res = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_WlanPolicySet(SL_NORMAL_POLICY) = %d", res);

    // Unregister mDNS services
    res = sl_NetAppMDNSUnRegisterService(0, 0);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_NetAppMDNSUnRegisterService() = %d", res);

    // Remove all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    res = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_WlanRxFilterSet(clear) = %d", res);

    return res;
#else
    return 0;
#endif
}

void sl_init()
{
#ifdef cc3200
    struct SL_STATE *w;

    w = sl_get();
    memset(w, 0, sizeof(struct SL_STATE));

    w->role = -1;
    w->device_ip = SL_WLAN_INVALID_ADDRESS;
    w->gateway_ip = SL_WLAN_INVALID_ADDRESS;
    w->delegate = NULL;

    //LOGLI("sl: init");
#endif
}

int sl_wlan_role_get()
{
#ifdef cc3200
    return sl_get()->role;
#else
    return SL_ROLE_STA;
#endif
}

int sl_wlan_role_set(int mode)
{
#ifdef cc3200
    int res;

    res = sl_WlanSetMode(mode);
    LOGL(res == 0 ? LOGGER_LEVEL_INFO : LOGGER_LEVEL_ERROR,
        "sl_WlanSetMode(mode=%d) = %d" , mode, res);

    return res;
#else
    return 0;
#endif
}

int sl_wlan_disconnect(void)
{
#ifdef cc3200
    int res;

    if (SL_ROLE_STA == sl_get()->role)
    {
        res = sl_WlanDisconnect();
        LOGL(res == 0 ? LOGGER_LEVEL_INFO : LOGGER_LEVEL_ERROR,
            "sl_WlanDisconnect() = %d" , res);
    }

    return res;
#else
    return 0;
#endif
}

int sl_wlan_is_connected(void)
{
#ifdef cc3200
    return sl_get()->connected;
#else
    return 1;
#endif
}

int sl_wlan_profile_remove(const short idx)
{
#ifdef cc3200
    int res;
    ASSERT(sl_get()->role != SL_ROLE_STOP);
    // make sure simplelink was started
    res = sl_WlanProfileDel(idx);
    LOGL(res == 0 ? LOGGER_LEVEL_INFO : LOGGER_LEVEL_ERROR,
        "sl_WlanProfileDel() = %d" , res);
    return res;
#else
    return 0;
#endif
}

unsigned char sl_wlan_profile_remove_set(unsigned char set)
{
#ifdef cc3200
    int idx;
    int err;
    unsigned char res = 0;
    unsigned char cur = set;

    for (idx = 0; idx < SL_WLAN_MAX_PROFILES; ++idx)
    {
        if ((0x80 & cur))
        {
            err = sl_WlanProfileDel(idx);
            res |= err ? 0 : 1;
            LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
                "sl_WlanProfileDel(%d) = %d" , idx, err);
        }

        cur <<= 1;
        res <<= 1;
    }

    LOGL(res == 0 ? LOGGER_LEVEL_INFO : LOGGER_LEVEL_ERROR,
        "sl_wlan_profile_remove_set(%02X) = %02X" , set, res);
    return res;
#else
    return set;
#endif
}


int sl_wlan_profile_count(unsigned char *profile_set)
{
#ifdef cc3200
    int i;
    int n;
    int res;
    unsigned char set;

    _i16 ssidlen;
    _i8 ssid[SL_SSID_LEN_MAX + 1];
    _u8 mac[6];
    SlSecParams_t sec;
    SlGetSecParamsExt_t secext;
    unsigned long priority;

    ASSERT(sl_get()->role != SL_ROLE_STOP);

    n = 0;
    set = 0;

    for (i = 0; i < SL_WLAN_MAX_PROFILES; ++i)
    {
        res = sl_WlanProfileGet(i, ssid, &ssidlen, mac, &sec, &secext, &priority);

        if (res >= 0)
        {
            ++n;
            ssid[ssidlen] = 0;
            LOGLI("sl: profile #%d ssid=[%s] security=%d priority=%d" , i, ssid, res, priority);
            set |= 1;
        }

        set <<= 1;
    }

    if (profile_set)
    {
        *profile_set = set;
    }

    return n;
#else
    return 0;
#endif
}

int sl_wlan_profile_add(const char *ssid, const char *psk)
{
#ifdef cc3200
    int res;
    int ret = 0;
    SlSecParams_t secParams;

    secParams.KeyLen = strlen(psk);

    // find first available index
    if (secParams.KeyLen == 0)
    {
        secParams.Key = NULL;
        secParams.Type = SL_SEC_TYPE_OPEN;

        res = sl_WlanProfileAdd((const _i8 *)ssid, strlen(ssid), NULL, (const SlSecParams_t *)&secParams, NULL, 0, 0);
        ret += (res >= 0) ? 1 : 0;
        SL_DEBUG("wlan: add profile open, res=%d" , res);
    }
    else if (secParams.KeyLen == 5 || secParams.KeyLen == 10       // 64-bit wep
            || secParams.KeyLen == 13 || secParams.KeyLen == 26     // 128-bit wep
            || secParams.KeyLen == 16 || secParams.KeyLen == 32)    // 152-bit wep
    {
        secParams.Key = (_i8 *)psk;
        secParams.Type = SL_SEC_TYPE_WEP;

        res = sl_WlanProfileAdd((const _i8 *)ssid, strlen(ssid), NULL, (const SlSecParams_t *)&secParams, NULL, 1, 0);
        ret += (res >= 0) ? 1 : 0;
        SL_DEBUG("wlan: add profile wep, res=%d" , res);
    }

    if (secParams.KeyLen >= 8 && secParams.KeyLen <= 63)
    {
        secParams.Key = (_i8 *)psk;
        secParams.Type = SL_SEC_TYPE_WPA_WPA2;

        res = sl_WlanProfileAdd((const _i8 *)ssid, strlen(ssid), NULL, (const SlSecParams_t *)&secParams, NULL, 2, 0);
        ret += (res >= 0) ? 1 : 0;
        SL_DEBUG("wlan: add profile wpa/wpa2, res=%d" , res);
    }

    return ret;
#else
    return 0;
#endif
}

int sl_wlan_profile_get(char *ssidout, int size)
{
#ifdef cc3200
    int res;
    _i16 ssidlen = 0;
    _i8 ssid[SL_SSID_LEN_MAX + 1];
    _u8 mac[6];
    SlSecParams_t sec;
    SlGetSecParamsExt_t secext;
    unsigned long priority;

    res = sl_WlanProfileGet(0, ssid, &ssidlen, mac, &sec, &secext, &priority);

    if (res >= 0) {
        ssid[ssidlen] = 0;
        strncpy(ssidout, (const char *)ssid, size);
    } else {
        ssid[0] = 0;
    }

    SL_DEBUG("wlan: get profile ssid=[%s], security=%d" , ssid, res);

    return res;
#else
    return -1;
#endif
}

int sl_wlan_initialize_ap(const char *ssid, unsigned char security, const char *psk)
{
#ifdef cc3200
    int res;

    res = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, strlen(ssid), (unsigned char*)ssid);
    SL_DEBUG("wifi: set AP ssid, res=%d" , res);

    res = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, sizeof(security), &security);
    SL_DEBUG("wifi: set AP security, res=%d" , res);

    if (SL_SEC_TYPE_OPEN != security)
    {
        res = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, strlen(psk), (unsigned char*)psk);
        SL_DEBUG("wifi: set AP psk, res=%d" , res);
    }

    UNUSED(res);
#endif
    return 0;
}

int sl_wlan_get_ap_ssid(char *ssid, unsigned short len)
{
#ifdef cc3200
    int res;
    _u16 opt;
    opt = WLAN_AP_OPT_SSID;
    res = sl_WlanGet(SL_WLAN_CFG_AP_ID, &opt, &len, (unsigned char *)ssid);
    SL_DEBUG("wifi: AP ssid get return res=%d [%s]" , res, ssid);
    return res;
#else
    return -1;
#endif
}

int sl_wlan_initialize_sta(void)
{
    return 0;
}

int sl_wlan_get_ssid(char ssid[32])
{
#ifdef cc3200
    strncpy(ssid, sl_get()->ssid, 32);
    return 0;
#else
    return -1;
#endif
}

int sl_wlan_get_mac(unsigned char mac[6])
{
#ifdef cc3200
    _u8 optu8;
    ASSERT(sl_get()->role != SL_ROLE_STOP);
    optu8 = SL_MAC_ADDR_LEN;
    return sl_NetCfgGet(SL_MAC_ADDRESS_GET, NULL, &optu8, (_u8 *)mac);
#else
    return -1;
#endif
}

int sl_wlan_get_bssid(unsigned char bssid[6])
{
#ifdef cc3200
    memcpy(bssid, sl_get()->bssid, 6);
    return 0;
#else
    return -1;
#endif
}

unsigned int sl_wlan_get_ip(void)
{
#ifdef cc3200
	return sl_get()->device_ip;
#else
	return 0;
#endif
}

unsigned int sl_wlan_get_gateway(void)
{
#ifdef cc3200
    return sl_get()->gateway_ip;
#else
	return 0;
#endif
}

unsigned int sl_wlan_get_dns(void)
{
#ifdef cc3200
    return sl_get()->dns_ip;
#else
	return 0;
#endif
}

int sl_file_load(const char *path, void *buf, int size)
{
#ifdef cc3200
    int res;
    _i32 handle = -1;

    res = sl_FsOpen((unsigned char *)path, FS_MODE_OPEN_READ, 0, &handle);
    LOGL(res < 0 ? LOGGER_LEVEL_ERROR : LOGGER_LEVEL_VERBOSE, 
        "sl_FsOpen(%s) = %d", path, res);

    if (res < 0)
    {
        goto cleanup_exit;
    }

    res = sl_FsRead(handle, 0, buf, size);
    LOGL(res < 0 ? LOGGER_LEVEL_ERROR : LOGGER_LEVEL_VERBOSE, 
        "sl_FsRead(%s, %d) = %d", path, size, res);

    if (res < 0)
    {
        goto cleanup_exit;
    }

cleanup_exit:
    if (-1 != handle)
    {
        sl_FsClose(handle, 0, 0, 0);
    }

    return res;
#else
    return -1;
#endif
}

int sl_file_save(const char *path, const void *buf, int size)
{
#ifdef cc3200
    int res;
    _i32 handle = -1;

    res = sl_FsOpen((unsigned char *)path,
        FS_MODE_OPEN_CREATE(ALIGN_N(size, CONFIG_CFG_FILE_SIZE_ALIGN),
        _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
        0,
        &handle);
    LOGL(res < 0 ? LOGGER_LEVEL_ERROR : LOGGER_LEVEL_VERBOSE, 
        "sl_FsOpen(%s) = %d", path, res);

    if (res < 0)
    {
        goto cleanup_exit;
    }

    res = sl_FsWrite(handle, 0, (_u8 *)buf, size);
    LOGL(res < 0 ? LOGGER_LEVEL_ERROR : LOGGER_LEVEL_VERBOSE, 
        "sl_FsWrite(%s, %d) = %d", path, size, res);

    if (res < 0)
    {
        goto cleanup_exit;
    }

cleanup_exit:
    if (-1 != handle)
    {
        sl_FsClose(handle, 0, 0, 0);
    }

    return res;
#else
    return -1;
#endif
}

int sl_file_length(const char *path)
{
#ifdef cc3200
    int res;
    SlFsFileInfo_t inf;

    res = sl_FsGetInfo((const unsigned char *)path, 0, &inf);

    if (0 == res)
    {
        return inf.FileLen;
    }
#endif
    return -1;
}

void sl_wlan_policy_set_long_sleep(int interval)
{
#ifdef cc3200
    int res;
    unsigned short policy[4] = {0, 0, interval, 0};

    ASSERT(sl_get()->role != SL_ROLE_STOP);

    if (0 != interval)
    {
        res = sl_WlanPolicySet(SL_POLICY_PM,
                SL_LONG_SLEEP_INTERVAL_POLICY,
                (unsigned char *)policy,
                sizeof(policy));

        //LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        //    "sl_WlanPolicySet(SL_LONG_SLEEP_INTERVAL_POLICY=%d) = %d", interval, res);
    }
    else
    {
        res = sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);
        //LOGL(res == 0 ? LOGGER_LEVEL_INFO : LOGGER_LEVEL_ERROR,
        //    "sl_WlanPolicySet(SL_NORMAL_POLICY) = %d", res);
    }
#endif
}

int sl_wlan_network_scan(int enable, int interval_sec)
{
#ifdef cc3200
    int res;
    res = sl_WlanPolicySet(SL_POLICY_SCAN,
            SL_SCAN_POLICY(enable),
            enable ? (unsigned char *)&interval_sec : NULL,
            enable ? sizeof(interval_sec) : 0);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_WlanPolicySet(scan=%d, interval=%d) = %d", enable, interval_sec, res);
    return res;
#else
    return -1;
#endif
}

int sl_wlan_rx_stats(void)
{
#ifdef cc3200
    int res;
    SlGetRxStatResponse_t rxStatResp;

    sl_WlanRxStatStart();
    osi_Sleep(2000);
    sl_WlanRxStatStop();
    res = sl_WlanRxStatGet(&rxStatResp , 0);
    if (0 == res)
    {
        LOGLI("AvarageDataCtrlRssi: %d", rxStatResp.AvarageDataCtrlRssi);
        LOGLI("AvarageMgMntRssi:    %d", rxStatResp.AvarageMgMntRssi);
    }
    return res;
#else
    return -1;
#endif
}

void sl_wlan_dump_hotspots(/*CC3200_WIFI_SCAN_CALLBACK callback*/)
{
#ifdef cc3200
    int i;
    int res;
    Sl_WlanNetworkEntry_t *entries;

    sl_wlan_network_scan(1, 1);

    entries = (Sl_WlanNetworkEntry_t *)malloc(sizeof(Sl_WlanNetworkEntry_t) * SL_WLAN_SCAN_MAX_ENTRIES);

    if (NULL == entries)
    {
        LOGLE("OOM: sl_wlan_dump_hotspots");
        return;
    }

    osi_Sleep(2000);

    res = sl_WlanGetNetworkList(0, SL_WLAN_SCAN_MAX_ENTRIES, entries);
    LOGLV("sl_WlanGetNetworkList() = %d", res);

    for (i = 0; i < res; ++i)
    {
        LOGLI("  Network[%d]: SSID=%s Security=%d RSSI=%d", i,
                entries[i].ssid,
                entries[i].sec_type,
                entries[i].rssi);
    }

    free(entries);

    sl_wlan_network_scan(0, 0);
#endif
}

void sl_netcfg_update_ip(void)
{
#ifdef cc3200
    int res;;
    struct SL_STATE *w;
    struct SL_EVENT_MSG msg;
    unsigned char ConfigOpt = 0;
    unsigned char ConfigLen = sizeof(SlNetCfgIpV4DhcpClientArgs_t);
    SlNetCfgIpV4DhcpClientArgs_t dhcp;

    UNUSED(dhcp);
    UNUSED(ConfigOpt);

    w = sl_get();

    if (SL_ROLE_STOP == sl_get()->role)
    {
        return;
    }

#if 0
    res = sl_NetCfgGet(SL_IPV4_DHCP_CLIENT, &ConfigOpt, &ConfigLen, (_u8 *)&dhcp);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "sl_NetCfgGet(SL_IPV4_DHCP_CLIENT) = %d", res);
    LOGL(res == 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "  ip=%08X gw=%08X mask=%08X dns1=%08X dns2=%08X srv=%08X lease=%lu state=%d",
        dhcp.Ip, dhcp.Gateway, dhcp.Mask, dhcp.Dns[0], dhcp.Dns[1], dhcp.DhcpServer, dhcp.LeaseTime, dhcp.DhcpState);
#endif

    _u8 dhcpIsOn = 0;
    ConfigLen = sizeof(SlNetCfgIpV4Args_t);
    SlNetCfgIpV4Args_t ipV4 = {0};

    res = sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &ConfigLen, (_u8 *)&ipV4);

    LOGL(res >= 0 ? LOGGER_LEVEL_VERBOSE : LOGGER_LEVEL_ERROR,
        "DHCP is %s IP %d.%d.%d.%d MASK %d.%d.%d.%d GW %d.%d.%d.%d DNS %d.%d.%d.%d res=%d",
        (dhcpIsOn > 0) ? "ON" : "OFF",
        SL_IPV4_BYTE(ipV4.ipV4,3),SL_IPV4_BYTE(ipV4.ipV4,2),SL_IPV4_BYTE(ipV4.ipV4,1),SL_IPV4_BYTE(ipV4.ipV4,0),
        SL_IPV4_BYTE(ipV4.ipV4Mask,3),SL_IPV4_BYTE(ipV4.ipV4Mask,2),SL_IPV4_BYTE(ipV4.ipV4Mask,1),SL_IPV4_BYTE(ipV4.ipV4Mask,0),
        SL_IPV4_BYTE(ipV4.ipV4Gateway,3),SL_IPV4_BYTE(ipV4.ipV4Gateway,2),SL_IPV4_BYTE(ipV4.ipV4Gateway,1),SL_IPV4_BYTE(ipV4.ipV4Gateway,0),
        SL_IPV4_BYTE(ipV4.ipV4DnsServer,3),SL_IPV4_BYTE(ipV4.ipV4DnsServer,2),SL_IPV4_BYTE(ipV4.ipV4DnsServer,1),SL_IPV4_BYTE(ipV4.ipV4DnsServer,0),
        res);

    if (res < 0)
    {
        return;
    }

    // IP address
    w->device_ip = ipV4.ipV4;
    w->gateway_ip = ipV4.ipV4Gateway;
    w->dns_ip = ipV4.ipV4DnsServer;

    msg.id = SL_EVENT_WLAN_IP_ACQUIRED;
    SL_FIRE_EVENT(w, &msg);
#endif
}

#ifdef cc3200
static void sl_ping_callback_wrapper(SlPingReport_t *r)
{
    struct SL_STATE *w = sl_get();

    if (r != (SlPingReport_t *)w->ping_report && NULL != w->ping_report && NULL != r)
    {
    	memcpy(w->ping_report, r, sizeof(SlPingReport_t));
    }

    //LOGL(LOGGER_LEVEL_DEBUG | LOGGER_LOCAL, "ping ip=%d.%d.%d.%d complete=%d/%d minrt=%d, maxrt=%d, avgrt=%d, testtime=%d",
    //        SPLIT_IP(p->ip_be), p->report.PacketsReceived, p->report.PacketsSent,
    //        p->report.MinRoundTime, p->report.MaxRoundTime, p->report.AvgRoundTime, p->report.TestTime);

    if (w->ping_callback)
    {
        w->ping_callback(w->ping_report);
    }


    w->ping_report = NULL;
    w->ping_callback = NULL;
}
#endif

int sl_netapp_ping(unsigned int ip_be, unsigned int interval_ms,
		unsigned char pkt_count, unsigned short pkt_size,
		unsigned short timeout_ms, SL_PING_CALLBACK callback, struct SL_PING_REPORT *report)
{
#ifdef cc3200
    struct SL_STATE *w = sl_get();
    SlPingStartCommand_t pingParams;

    if (NULL != w->ping_report
        || SL_ROLE_STOP == w->role
        || SL_WLAN_INVALID_ADDRESS == w->gateway_ip)
    {
        return -1;
    }

    w->ping_report = report;
    w->ping_callback = callback;

    // Set the ping parameters
    memset(&pingParams, 0, sizeof(pingParams));
    pingParams.PingIntervalTime = interval_ms;
    pingParams.PingSize = pkt_size;
    pingParams.PingRequestTimeout = timeout_ms;
    pingParams.TotalNumberOfAttempts = pkt_count;
    pingParams.Flags = 0;
    pingParams.Ip = ip_be;

    // Check for LAN connection
    return sl_NetAppPingStart(&pingParams, SL_AF_INET, (SlPingReport_t *)report, sl_ping_callback_wrapper);
#else
    return -1;
#endif
}

#ifdef cc3200
//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    struct SL_STATE *w;
    struct SL_EVENT_MSG msg;

    w = sl_get();

    switch (pWlanEvent->Event)
    {
    case SL_WLAN_STA_CONNECTED_EVENT:
        sl_get()->role = SL_ROLE_AP;
        msg.id = SL_EVENT_STA_CONNECTED;
        SL_FIRE_EVENT(w, &msg);
        break;

    case SL_WLAN_STA_DISCONNECTED_EVENT:
        sl_get()->role = SL_ROLE_AP;
        msg.id = SL_EVENT_STA_DISCONNECTED;
        SL_FIRE_EVENT(w, &msg);
        break;

    case SL_WLAN_CONNECT_EVENT:
    {
        // Notifies that STA is connected
        //
        // Information about the connected AP (like name, MAC etc) will be
        // available in 'sl_protocol_wlanConnectAsyncResponse_t'-Applications
        // can use it if required
        //
        //  sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;
        // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
        //
        sl_get()->role = SL_ROLE_STA;
        w->connected = 1;
        // Copy new connection SSID and BSSID to global parameters
        memcpy(w->ssid,
                pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_name,
                pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
        memcpy(w->bssid, pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid, SL_BSSID_LENGTH);
        msg.id = SL_EVENT_WLAN_CONNECTED;
        SL_FIRE_EVENT(w, &msg);
        break;
    }

    case SL_WLAN_DISCONNECT_EVENT:
    {
        slWlanConnectAsyncResponse_t* pEventData = NULL;

        sl_get()->role = SL_ROLE_STA;
        pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

        // If the user has initiated 'Disconnect' request,
        //'reason_code' is SL_USER_INITIATED_DISCONNECTION
        if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
        {
            msg.id = SL_EVENT_WLAN_USER_INITIATED_DISCONNECT;
            SL_FIRE_EVENT(w, &msg);
        }
        else
        {
            msg.id = SL_EVENT_WLAN_DISCONNECT;
            SL_FIRE_EVENT(w, &msg);
        }

        w->connected = 0;
        w->device_ip = SL_WLAN_INVALID_ADDRESS;
        w->gateway_ip = SL_WLAN_INVALID_ADDRESS;
        //memset(w->ssid, 0, sizeof(w->ssid));
        //memset(w->bssid, 0, sizeof(w->bssid));
        break;
    }

    case SL_WLAN_CONNECTION_FAILED_EVENT:
        sl_get()->role = SL_ROLE_STA;
        msg.id = SL_EVENT_WLAN_CONNECT_FAILED;
        SL_FIRE_EVENT(w, &msg);
        break;

    default:
        LOGLE("[WLAN] Unexpected event [0x%x]" , pWlanEvent->Event);
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    struct SL_STATE *w;
    struct SL_EVENT_MSG msg;

    w = sl_get();

    switch (pNetAppEvent->Event)
    {
    case SL_NETAPP_IP_LEASED_EVENT:
        w->ap_client_ip = pNetAppEvent->EventData.ipLeased.ip_address;
        msg.id = SL_EVENT_STA_IP_LEASED;
        msg.u.sta_ip.ip_be = w->ap_client_ip;
        SL_FIRE_EVENT(w, &msg);
        break;
    case SL_NETAPP_IP_RELEASED_EVENT:
        msg.id = SL_EVENT_STA_IP_RELEASED;
        msg.u.sta_ip.ip_be = w->ap_client_ip;
        SL_FIRE_EVENT(w, &msg);
        break;
    case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            // IP address
            w->device_ip = pEventData->ip;
            w->gateway_ip = pEventData->gateway;
            w->dns_ip = pEventData->dns;

            msg.id = SL_EVENT_WLAN_IP_ACQUIRED;
            msg.u.wlan_ip.ip_be = pNetAppEvent->EventData.ipAcquiredV4.ip;
            msg.u.wlan_ip.gw_be = pNetAppEvent->EventData.ipAcquiredV4.gateway;
            msg.u.wlan_ip.dns_be = pNetAppEvent->EventData.ipAcquiredV4.dns;
            SL_FIRE_EVENT(w, &msg);
            break;
        }

    default:
        LOGLE("[NETAPP] Unexpected event [0x%x]", pNetAppEvent->Event);
        break;
    }
}

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent, SlHttpServerResponse_t *pHttpResponse)
{
}

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    struct SL_EVENT_MSG msg;
    struct SL_STATE *w = sl_get();

    if (!pDevEvent)
    {
        return;
    }

    msg.id = SL_EVENT_GENERAL_EVENT;
    msg.u.general_event.event = pDevEvent->Event;
    switch (pDevEvent->Event)
    {
    case SL_DEVICE_ABORT_ERROR_EVENT:
        msg.u.general_event.u.abort.type = pDevEvent->EventData.deviceReport.AbortType;
        msg.u.general_event.u.abort.data = pDevEvent->EventData.deviceReport.AbortData;
        SL_FIRE_EVENT(w, &msg);
        break;
    case SL_DEVICE_GENERAL_ERROR_EVENT:
        msg.u.general_event.u.general.status = pDevEvent->EventData.deviceEvent.status;
        msg.u.general_event.u.general.sender = pDevEvent->EventData.deviceEvent.sender;
        SL_FIRE_EVENT(w, &msg);        break;
    case SL_DEVICE_DRIVER_ASSERT_ERROR_EVENT:
    case SL_DEVICE_DRIVER_TIMEOUT_CMD_COMPLETE:
    case SL_DEVICE_DRIVER_TIMEOUT_SYNC_PATTERN:
    case SL_DEVICE_DRIVER_TIMEOUT_ASYNC_EVENT:
    default:
        msg.u.general_event.u.info = pDevEvent->EventData.deviceDriverReport.info;
        SL_FIRE_EVENT(w, &msg);
        break;
    }
}

void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    struct SL_EVENT_MSG msg;
    struct SL_STATE *w = sl_get();
    msg.id = SL_EVENT_SOCKET_EXCEPTION;
    msg.u.socket_exception.event = pSock->Event;
    msg.u.socket_exception.fd = pSock->socketAsyncEvent.SockTxFailData.sd;
    msg.u.socket_exception.status = pSock->socketAsyncEvent.SockTxFailData.status;
    SL_FIRE_EVENT(w, &msg);
}
#endif // cc3200
