//
// common include files
//
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

//
// sdk include files
//
#include "cfg.h"


#include "iotk/iotk_api.h"
#include "iotk/iotk_p2p.h"

#include "iotk/iotk_pal.h"

#include "logger/logger.h"
#include "stream/streambuf.h"
#include "msgpack/msgpack.h"
#include "msgpack/msgpack_reader.h"
#include "msgpack/msgpack_writer.h"


#include "common.h"
#include "sl.h"

#define FLAG_AP_PREPARING           0x00000001
#define FLAG_AP_STANDBY             0x00000002
#define FLAG_STA_CONNECTING         0x00000004
#define FLAG_STA_CONNECTED          0x00000008
#define FLAG_IP_ACQUIRED            0x00000010
#define FLAG_IOTK_CONNECTED         0x00000020
#define FLAG_IOTK_LOGGED_ON         0x00000080

struct APP_CONTEXT
{
    //
    // do not modify context owned by other task without lock
    //
    int flags;
    char last_p2p_caller[IOTK_CID_LENGTH + 1];
    unsigned int last_p2p_call_id;
    struct IOTK_INTERFACE_DEVICE *server;
};

struct IOTK_SYNC_MSG
{
    void *ap;
    unsigned int ap_size;
    iotk_event_t signal;
};

static struct APP_CONTEXT _app;

static struct APP_CONTEXT *app_get()
{
    return &_app;
}

static void _app_get_dp_writer(struct IWSTREAM *s, void *ctx)
{
    //
    // datapoint 1
    //
    MsgpackPackArray(s, 2);
    // datapoint id
    MsgpackPackUnsigned(s, IOTK_DPID_BASE_SW_VERSION);
    // datapoing timestamp
    MsgpackPackUnsigned(s, 0);

    //
    // datapoint 2
    //
    MsgpackPackArray(s, 2);
    // datapoint id
    MsgpackPackUnsigned(s, IOTK_DPID_BASE_SYS_VERSION);
    // datapoing timestamp
    MsgpackPackUnsigned(s, 0);
}


static void _app_do_get_dp(struct APP_CONTEXT *d)
{
    // get data from server
    d->server->get_dp(&d->server->iunknown, 
        200,    // msg sequence: 200
        1,      // result count limit for each data
        1,      // result order ?
        2,      // datapoint id count
        _app_get_dp_writer, NULL);
}

static void _app_set_dp_writer(struct IWSTREAM *s, void *ctx)
{
    int n;
    char buffer[32];

    //
    // datapoint 1
    //
    MsgpackPackArray(s, 3);
    // id
    MsgpackPackUnsigned(s, IOTK_DPID_BASE_SW_VERSION);
    // timestamp
    MsgpackPackUnsigned(s, 0);
    // value & length, you may use msgpack to store complex data
    n = sprintf(buffer, "s/w ver: %s", __DATE__);
    MsgpackPackString(s, buffer, n);

    //
    // datapoint 2
    //
    MsgpackPackArray(s, 3);
    // id
    MsgpackPackUnsigned(s, IOTK_DPID_BASE_SYS_VERSION);
    // timestamp
    MsgpackPackUnsigned(s, 0);
    // value & length, you may use msgpack to store complex data
    n = sprintf(buffer, "sys ver: %s", __TIME__);
    MsgpackPackString(s, buffer, n);
}

static void _app_do_set_dp(struct APP_CONTEXT *d)
{
    // get data from server
    d->server->set_dp(&d->server->iunknown, 
        100,    // msg sequence: 100
        2,      // datapoint id count
        _app_set_dp_writer, NULL);
}

static void _app_on_login_ack(struct APP_CONTEXT *d, struct IOTK_EVENT *e)
{
    if (e->u.logged_in.err_code != 0)
    {
        LOGLE("login failed: %d", e->u.logged_in.err_code);
        return;
    }

    LOGLI("login successful");

    d->flags |= FLAG_IOTK_LOGGED_ON;

    _app_do_set_dp(d);
    _app_do_get_dp(d);
}

static void _app_on_get_dp_ack(struct APP_CONTEXT *d, struct IOTK_EVENT *e)
{
    unsigned int i;
    unsigned int ii;
    unsigned int items;
    unsigned long long dpid;
    struct MSGPACK_OBJECT o;
    struct IRSTREAM *irs = e->u.get_dp_ack.irs;

    LOGLI("get dp ack, seq=%llu", e->u.get_dp_ack.seq);

    for (i = 0; i < e->u.get_dp_ack.count; ++i)
    {
        // dpid
        MsgpackObjectGetOne(irs, &o);
        dpid = MsgpackGetUnsigned(&o, 0);
        // array of items
        MsgpackObjectGetOne(irs, &o);
        items = MsgpackGetArrayLength(&o);

        LOGLI("  dpid=%llu, items=%d", dpid, items);

        for (ii = 0; ii < items; ++ii)
        {
            // array of timestamp/value pair, fixed length 2
            MsgpackObjectGetOne(irs, &o);
            // timestamp
            MsgpackObjectGetOne(irs, &o);
            // value
            MsgpackObjectGetOne(irs, &o);

            if (MsgpackIsString(&o))
            {
                LOGLI("    value=[%.*s]", 
                    MsgpackGetStringLength(&o), 
                    MsgpackGetStringPointer(&o));
            }
        }
    }
}

static void _app_on_set_dp_ack(struct APP_CONTEXT *d, struct IOTK_EVENT *e)
{
    int res;
    unsigned int i;
    unsigned long long dpid;
    unsigned long long ts;
    struct MSGPACK_OBJECT o;
    struct IRSTREAM *irs = e->u.set_dp_ack.irs;

    LOGLI("set dp ack, seq=%llu", e->u.set_dp_ack.seq);

    for (i = 0; i < e->u.set_dp_ack.count; ++i)
    {
        // array of dp items
        MsgpackObjectGetOne(irs, &o);
        
        if (3 != MsgpackGetArrayLength(&o))
        {
            LOGLI("invalid format!");
            break;
        }

        // dpid
        MsgpackObjectGetOne(irs, &o);
        dpid = MsgpackGetUnsigned(&o, 0);
        // timestamp
        MsgpackObjectGetOne(irs, &o);
        ts = MsgpackGetUnsigned(&o, 0);
        // result
        MsgpackObjectGetOne(irs, &o);
        res = (int)MsgpackGetSigned(&o, 0);
        LOGLI("  dpid=%llu, ts=%llu, result=%d", dpid, ts, res);
    }
}

static void _app_on_iotk_connected(struct APP_CONTEXT *c, struct IOTK_EVENT *e)
{
    c->flags |= FLAG_IOTK_CONNECTED;
    c->server->login(&c->server->iunknown, 1, IOTK_TEST_PID, IOTK_TEST_SN, IOTK_TEST_SIG, IOTK_TEST_CID);

    LOGLI("iotk connected, send login request");
}

static void _app_on_iotk_disconnected(struct APP_CONTEXT *d, struct IOTK_EVENT *e)
{
    // clear all flags
    d->flags = 0;

    LOGLI("iotk disconnected");
}

static void _app_on_iotk_p2p_setup(struct APP_CONTEXT *c, struct IOTK_EVENT *e)
{
    // store call information
    strncpy(c->last_p2p_caller, e->u.p2p_setup.caller, IOTK_CID_LENGTH);
    c->last_p2p_caller[IOTK_CID_LENGTH] = 0;
    c->last_p2p_call_id = e->u.p2p_setup.call_id;

    // accept p2p
    iotk_p2p_accept_call(e->u.p2p_setup.channel, e->u.p2p_setup.peer);

    // response
    c->server->p2p_setup_ack(&c->server->iunknown, e->u.p2p_setup.seq + 1, e->u.p2p_setup.caller);
}

static void _app_on_iotk_p2p_sdp(struct APP_CONTEXT *c, struct IOTK_EVENT *e)
{
    iotk_p2p_update_peer_addr(e->u.p2p_sdp.sdp);
}

static void _app_on_iotk_p2p_disconnect(struct APP_CONTEXT *c, struct IOTK_EVENT *e)
{
    c->last_p2p_caller[0] = 0;
    c->last_p2p_call_id = 0;
}

static void _app_on_iotk_event(void *sender, struct IOTK_EVENT *e)
{
    struct APP_CONTEXT *c = app_get();
    LOGLD("get iotk event id=%d", e->id);

    switch (e->id)
    {
    case IOTK_EVENT_CONNECTED:
        _app_on_iotk_connected(c, e);
        break;
    case IOTK_EVENT_DISCONNECTED:
        _app_on_iotk_disconnected(c, e);
        break;
    case IOTK_EVENT_LOGIN_ACK:
        _app_on_login_ack(c, e);
        break;
    case IOTK_EVENT_REGISTER_ACK:
        // TODO: store cid to device flash
        // TODO: login with new CID
        break;
    case IOTK_EVENT_BIND_CID_ACK:
        break;
    case IOTK_EVENT_GET_DP_ACK:
        _app_on_get_dp_ack(c, e);
        break;
    case IOTK_EVENT_SET_DP_ACK:
        _app_on_set_dp_ack(c, e);
        break;
    case IOTK_EVENT_P2P_SETUP:
        _app_on_iotk_p2p_setup(c, e);
        break;
    case IOTK_EVENT_P2P_SDP:
        _app_on_iotk_p2p_sdp(c, e);
        break;
    case IOTK_EVENT_P2P_DISCONNECT:
        _app_on_iotk_p2p_disconnect(c, e);
        break;
    }
}

static void _app_do_send_sdp_update(void *p)
{
    struct APP_CONTEXT *ctx = app_get();
    struct IOTK_SYNC_MSG *msg = (struct IOTK_SYNC_MSG *)p;
    struct IOTK_INTERFACE_DEVICE *srv = app_get()->server;

    srv->p2p_sdp_update((struct IOTK_IUNKNOWN *)srv, 0, 
        ctx->last_p2p_caller,
        ctx->last_p2p_call_id,
        (struct P2P_SDP *)msg->ap);

    iotk_event_set(&msg->signal);
}

static void _app_on_iotk_p2p_sdp_update(struct IOTK_P2P_EVENT *e)
{
    struct IOTK_SYNC_MSG msg;

    iotk_event_create(&msg.signal);

    msg.ap = e->u.sdp_update = e->u.sdp_update;
    msg.ap_size = 1;

    //
    // we have to send sdp update data in iotk thread context
    //
    iotk_spawn(_app_do_send_sdp_update, &msg);
    
    // wait until sdp data sent
    iotk_event_wait(&msg.signal, IOTK_WAIT_FOREVER);
    iotk_event_delete(&msg.signal);
}

static void _app_on_iotk_p2p_event(struct IOTK_P2P_EVENT *e)
{
    LOGLI("on p2p event = %d", e->id);

    switch (e->id)
    {
    case IOTK_P2P_EVENT_SDP_UPDATE:
        _app_on_iotk_p2p_sdp_update(e);
        break;
    case IOTK_P2P_EVENT_CALL_ERROR:
        LOGLE("p2p call error: %d", e->u.call_error);
        break;
    }
}

static void _app_on_iotk_p2p_data_incoming(const void *data, unsigned int len)
{
    //LOGLV("p2p incoming data...");
}

static int _app_on_iotk_p2p_data_outgoing(int (*p2p_send)(const char *buf, unsigned int len))
{
    //LOGLV("p2p outgoing data...");
    return 0;
}

static void _app_p2p_task(void *unused)
{
    UNUSED(unused);

    for (;;)
    {
        iotk_p2p_runloop();
    }
}

static void _app_p2p_get_timestamp(struct timeval *tv)
{
#ifdef _WIN32
    DWORD dwTickCount = GetTickCount();
    tv->tv_sec = dwTickCount / 1000;
    tv->tv_usec = (dwTickCount % 1000) * 1000;
#elif defined(cc3200)
    // TODO: ...
#endif
}

static void _app_on_sl_event(void *p)
{
    struct SL_EVENT_MSG *e = (struct SL_EVENT_MSG *)p;

    LOGLI("on sl event = %d", e->id);

    switch (e->id)
    {
    case SL_EVENT_STA_CONNECTED:
    case SL_EVENT_STA_DISCONNECTED:
    case SL_EVENT_STA_IP_LEASED:
    case SL_EVENT_STA_IP_RELEASED:
        break;
    case SL_EVENT_WLAN_CONNECTED:
        break;
    case SL_EVENT_WLAN_CONNECT_FAILED:
        break;
    case SL_EVENT_WLAN_DISCONNECT:
        break;
    case SL_EVENT_WLAN_IP_ACQUIRED:
        break;
    }

    free(e);
}

static void _sl_event_callback(struct SL_EVENT_MSG *e)
{
    struct SL_EVENT_MSG *dup;

    dup = (struct SL_EVENT_MSG *)malloc(sizeof(struct SL_EVENT_MSG));
    if (NULL != dup)
    {
        memcpy(dup, e, sizeof(struct SL_EVENT_MSG));
        iotk_spawn(_app_on_sl_event, dup);
    }
}

void application_main(void *unused)
{
    iotk_task_t p2p_task;
    const char *server_addr;
    
    UNUSED(unused);

    // create app instance
    memset(app_get(), 0, sizeof(_app));

    sl_init();
    sl_start();

    cfg_init();

    // reset wifi config
    sl_reset();
    sl_wlan_role_set(SL_ROLE_STA);

    // set wifi profile
    sl_wlan_profile_remove(0xFF);
    sl_wlan_profile_add(SSID_NAME, SECURITY_KEY);

    // restart simplelink
    sl_stop();
    sl_set_delegate(_sl_event_callback);
    sl_start();

    // simulation of cfg_load
    cfg_set(CFG_ID_CID, 0, IOTK_TEST_CID, strlen(IOTK_TEST_CID));
    cfg_set(CFG_ID_MSG_SERVER_ADDR, 0, IOTK_TEST_SERVER, strlen(IOTK_TEST_SERVER));
    cfg_set(CFG_ID_LOG_SERVER_ADDR, 0, IOTK_TEST_LOG_SERVER, strlen(IOTK_TEST_LOG_SERVER));

    iotk_init();

    // load value from config
    server_addr = cfg_get_ptr(CFG_ID_MSG_SERVER_ADDR);
    iotk_cfg_set(IOTK_CFG_SERVER_ADDR, strlen(server_addr), server_addr);
    iotk_cfg_set(IOTK_CFG_EVENT_CALLBACK, sizeof(void *), _app_on_iotk_event);
    //iotk_start();

    // get and save server interface
    app_get()->server = (struct IOTK_INTERFACE_DEVICE *)
        iotk_query_interface(&iotk_iid_device);

    //
    // initialize p2p component
    //
    iotk_p2p_init();
    iotk_p2p_setopt(IOTK_P2P_OPT_GET_TIMESTAMP_CALLBACK, _app_p2p_get_timestamp);
    iotk_p2p_setopt(IOTK_P2P_OPT_EVENT_CALLBACK, _app_on_iotk_p2p_event);
    iotk_p2p_setopt(IOTK_P2P_OPT_DATA_OUTGOING_CALLBACK, _app_on_iotk_p2p_data_outgoing);
    iotk_p2p_setopt(IOTK_P2P_OPT_DATA_INCOMING_CALLBACK, _app_on_iotk_p2p_data_incoming);

    // create p2p thread
    iotk_task_create(_app_p2p_task, NULL, 512, NULL, 1, &p2p_task);

    for (;;)    
    {
        iotk_run(1000);
        LOGLD("iotk running...");
    }
}

int main()
{
    platform_init();
    return 0;
}
