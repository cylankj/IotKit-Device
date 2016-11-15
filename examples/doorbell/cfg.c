#include <string.h>

#include "cfg.h"

#ifndef MIN
#define MIN(a, b)                   ((a) < (b) ? (a) : (b))
#endif

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(x)     (sizeof(x) / sizeof((x)[0]))
#endif

static unsigned long long _cfg_dpid[] =
{
#define CFG_DECLARE_VID(id, dpid, len)  dpid,
    CONFIG_TABLE(CFG_DECLARE_VID)
#undef CFG_DECLARE_VID
};

static unsigned int _cfg_len[] =
{
#define CFG_DECLARE_VLEN(id, dpid, len)  len,
    CONFIG_TABLE(CFG_DECLARE_VLEN)
#undef CFG_DECLARE_VLEN
};

struct CFG_RAM
{
    struct
    {
#define CFG_DECLARE_NODES(id, dpid, len)  struct CONFIG_NODE id##_node;
        CONFIG_TABLE(CFG_DECLARE_NODES)
#undef CFG_DECLARE_NODES
    } nodes;
    struct
    {
#define CFG_DECLARE_SPACE(id, dpid, len)  char id##_placeholder[len];
        CONFIG_TABLE(CFG_DECLARE_SPACE)
#undef CFG_DECLARE_SPACE
    } spaces;
};
    
static struct CFG_RAM _cfg;

int cfg_set(int id, unsigned long long ts, const char *val, int length)
{
    int i;
    int res;
    unsigned char *p;
    struct CONFIG_NODE *node;

    p = (unsigned char *)&_cfg;

    for (i = 0; i < ARRAY_LENGTH(_cfg_len); ++i)
    {
        node = (struct CONFIG_NODE *)p;
        if (id == node->id)
        {
            res = MIN(length, (int)_cfg_len[i]);
            memcpy(&node[1], val, res);
            return res;
        }
        p += sizeof(struct CONFIG_NODE) + _cfg_len[i];
    }

    // dp id not found
    return 0;
}

int cfg_get(int id, unsigned long long *ts, char *val, int length)
{
    int i;
    int res;
    unsigned char *p;
    struct CONFIG_NODE *node;

    p = (unsigned char *)&_cfg;

    for (i = 0; i < ARRAY_LENGTH(_cfg_len); ++i)
    {
        node = (struct CONFIG_NODE *)p;
        if (id == node->id)
        {
            if (ts)
            {
                *ts = node->ts;
            }
            if (val && length)
            {
                res = MIN(length, (int)_cfg_len[i]);
                memcpy(val, &node[1], res);
                return res;
            }
        }
        p += sizeof(struct CONFIG_NODE) + _cfg_len[i];
    }

    // dp id not found
    return 0;
}

void *cfg_get_ptr(int id)
{
    int i;
    unsigned char *p;
    struct CONFIG_NODE *node;

    p = (unsigned char *)&_cfg;

    for (i = 0; i < ARRAY_LENGTH(_cfg_len); ++i)
    {
        node = (struct CONFIG_NODE *)p;
        if (id == node->id)
        {
            return (void *)&node[1];
        }
        p += sizeof(struct CONFIG_NODE) + _cfg_len[i];
    }

    return NULL;
}

int cfg_id_from_dpid(unsigned long long dpid)
{
    int i;
    for (i = 0; i < ARRAY_LENGTH(_cfg_dpid); ++i)
    {
        if (dpid == _cfg_dpid[i])
        {
            return i;
        }
    }
    return -1;
}

unsigned long long cfg_id_to_dpid(int id)
{
    if (id > 0 && id < ARRAY_LENGTH(_cfg_dpid))
    {
        return _cfg_dpid[id];
    }
    return 0;
}

int cfg_init(void)
{
    int i;
    unsigned char *p;
    struct CONFIG_NODE *node;

    memset(&_cfg, 0, sizeof(_cfg));
    p = (unsigned char *)&_cfg;

    for (i = 0; i < ARRAY_LENGTH(_cfg_len); ++i)
    {
        node = (struct CONFIG_NODE *)p;
        node->id = i;
        node->ts_present = 1;
        node->length = _cfg_len[i];
        node->ts = 0;
        p += sizeof(struct CONFIG_NODE) + _cfg_len[i];
    }

    return 0;
}
