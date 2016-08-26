#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "iotk_api.h"
#include "msgpack/msgpack.h"

#define IOTK_TEST_SERVER    "yf.jfgou.com"
#define IOTK_TEST_VID       "00A8"
#define IOTK_TEST_CID       "00A8006Bf1ca6007e03751b1e32f287b56fb810a"
#define IOTK_TEST_PID       107
#define IOTK_TEST_SN        "67E0BDE3332CAE73"
#define IOTK_TEST_SIG       "4E93C9264E11FF04A0444D082A70A8585AA175D516FF1C5EB1858B16849C168B116AD51FA149B0DF1434620F135167BF53B43D537690BE966D79E8DD0B01D6F6"

#define IOTK_TEST_CID_OLD   "200000000001"

#define FLAG_CONNECTED      0x00000001
#define FLAG_LOGGED_IN      0x00000002

struct IOTK_DEMO
{
    int flags;
    int gp_timer;
    HANDLE msgq_in;
    HANDLE msgq_out;
};

static struct IOTK_DEMO _demo;

static void init(void)
{
#ifdef _WIN32
    WSADATA wsd;
    WSAStartup(MAKEWORD(1, 0), &wsd);
#endif
}

static DWORD WINAPI _iotk_thread(void *p)
{
    for (;;)
    {
        iotk_run(NULL);
    }
}

static void _iotk_event_callback(const struct IOTK_EVENT *e)
{
    WriteFile(_demo.msgq_out, e, sizeof(struct IOTK_EVENT), NULL, NULL);
}

static void _iotk_on_timer(struct IOTK_DEMO *d, struct IOTK_EVENT *e)
{
    printf("timer timeout, now=%d\n", time(NULL));

    // TODO: check wifi connection ?

    iotk_timer_set_expiration(d->gp_timer, GetTickCount() + 10000);
}

static void _iotk_on_connected(struct IOTK_DEMO *d, struct IOTK_EVENT *e)
{
    d->flags |= FLAG_CONNECTED;
    //iotk_rolink_login(IOTK_TEST_PID, IOTK_TEST_SN, IOTK_TEST_SIG, IOTK_TEST_CID);
    iotk_rolink_compat_login(IOTK_TEST_PID, IOTK_TEST_CID_OLD, 1, 0, "SSID", "SoftVER", "SysVER", "MAC");
}

static void _iotk_on_disconnected(struct IOTK_DEMO *d, struct IOTK_EVENT *e)
{
    // clear all flags
    d->flags = 0;
}

static void _iotk_on_login(struct IOTK_DEMO *d, struct IOTK_EVENT *e)
{
    struct IOTK_DP_KEY keys[2];

    if (e->u.logged_in.err_code != 0)
    {
        printf("login failed: %d\n", e->u.logged_in.err_code);
        return;
    }

    d->flags |= FLAG_LOGGED_IN;

    // retrive device settings
    keys[0].dpid = IOTK_DPID_CAMERA_WARN_SETTINGS;
    keys[0].ts = 0;

    keys[1].dpid = IOTK_DPID_CAMERA_WARN_TIME;
    keys[1].ts = 0;

    iotk_rolink_get_dp(1, 10, 1, keys, 2);
}

static void _iotk_on_get_dp_ack(struct IOTK_DEMO *d, struct IOTK_EVENT *e)
{
    size_t n;
    MSGPACK_OBJECT *o;
    MSGPACK_UNPACKER *up;

    up = MsgpackUnpackerNew();
    MsgpackUnpackerBufferReserve(up, e->u.get_dp_ack.length);
    memcpy(MsgpackUnpackerBufferGet(up), e->u.get_dp_ack.data, e->u.get_dp_ack.length);
    MsgpackUnpackerBufferConsume(up, e->u.get_dp_ack.length);

    if (MsgpackUnpackerPrepare(up))
    {
        n = MsgpackUnpackerObjects(up);
        o = (MSGPACK_OBJECT *)malloc(n * sizeof(MSGPACK_OBJECT));

        if (NULL != o)
        {
            MsgpackUnpackerUnpack(up, o, n);

            free(o);
        }
    }

    MsgpackUnpackerFree(up);
}

unsigned int iotk_time_tick(void)
{
    return GetTickCount();
}

int main()
{
    int n;
    int res;
    BOOL bres;
    struct IOTK_DP dp;
    struct IOTK_EVENT e;

    init();
    iotk_init();
    iotk_cfg_set(IOTK_CFG_SERVER_ADDR, sizeof(IOTK_TEST_SERVER), IOTK_TEST_SERVER);
    iotk_cfg_set(IOTK_CFG_EVENT_CALLBACK, sizeof(void *), _iotk_event_callback);
    iotk_start();

    // create instance
    memset(&_demo, 0, sizeof(_demo));

    // initialize msg queue
    if (!CreatePipe(&_demo.msgq_in, &_demo.msgq_out, NULL, sizeof(struct IOTK_EVENT) * 16))
    {
        printf("create queue failed.\n");
        return -1;
    }

    // create iotk task
    CreateThread(NULL, 2048, _iotk_thread, NULL, 0, NULL);

    // create timer
    _demo.gp_timer = iotk_timer_new(GetTickCount() + 10000);

    memset(&e, 0, sizeof(e));

    for (;;)
    {
        bres = ReadFile(_demo.msgq_in, &e, sizeof(e), &n, NULL);
        if (!bres)
        {
            printf("read queue failed: %d\n", GetLastError());
            break;
        }

        printf("get iotk event id=%d, iotk flags=%d\n", e.id, _demo.flags);
        switch (e.id)
        {
        case IOTK_EVENT_TIMER_TIMEOUT:
            _iotk_on_timer(&_demo, &e);
            break;
        case IOTK_EVENT_ROLINK_CONNECTED:
            _iotk_on_connected(&_demo, &e);
            break;
        case IOTK_EVENT_ROLINK_DISCONNECTED:
            _iotk_on_disconnected(&_demo, &e);
            break;
        case IOTK_EVENT_ROLINK_LOGIN_ACK:
            _iotk_on_login(&_demo, &e);
            break;
        case IOTK_EVENT_ROLINK_REGISTER_ACK:
            // TODO: store cid to device flash
            // TODO: login with new CID
            break;
        case IOTK_EVENT_ROLINK_BIND_CID_ACK:
            break;
        case IOTK_EVENT_ROLINK_GET_DP_ACK:
            _iotk_on_get_dp_ack(&_demo, &e);
            break;
        case IOTK_EVENT_ROLINK_SET_DP_ACK:
            break;
        case IOTK_EVENT_ROLINK_DEL_DP_ACK:
            break;
        }
    }

    return 0;
}
