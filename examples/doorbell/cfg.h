#ifndef __CFG_H__
#define __CFG_H__

#include "iotk/iotk_api.h"

#ifndef ALIGN4
#define ALIGN4(x)   (((long)x + 3) & ~3)
#endif

#define CONFIG_TABLE(_) \
    _(CID, 0, IOTK_CID_LENGTH + 1) \
    _(SW_VERSION, IOTK_DPID_BASE_SW_VERSION, sizeof(int)) \
    _(SYS_VERSION, IOTK_DPID_BASE_SYS_VERSION, sizeof(int)) \
    _(MSG_SERVER_ADDR, IOTK_DPID_DEVICE_SERVER, 32) \
    _(MSG_SERVER_PORT, 0, sizeof(int)) \
    _(LOG_SERVER_ADDR, IOTK_DPID_DEVICE_LOG_SERVER, 32) \
    _(LOG_SERVER_PORT, 0, sizeof(int))

enum _CFG_ID
{
#define CFG_DECLARE_ID(id, dpid, len)  CFG_ID_##id,
    CONFIG_TABLE(CFG_DECLARE_ID)
#undef CFG_DECLARE_ID
};

#pragma pack(push, 4)
struct CONFIG_NODE
{
    // config id
    int id;
    // length of value, align to 4 bytes
    unsigned ts_present:1, length: 31;
    // timestamp
    unsigned long long ts;
};
#pragma pack(pop)

int cfg_set(int id, unsigned long long ts, const char *val, int length);

int cfg_get(int id, unsigned long long *ts, char *val, int length);

int cfg_id_from_dpid(unsigned long long dpid);

unsigned long long cfg_id_to_dpid(int id);

void *cfg_get_ptr(int id);

int cfg_init(void);

#endif // __CFG_H__
