#ifndef __IOTK_P2P_H__
#define __IOTK_P2P_H__

#include "iotk_api.h"

enum IOTK_P2P_CALL_ERROR
{
    /* P2P initiate error */
    IOTK_P2P_ERROR_NONE = 0,
    IOTK_P2P_ERROR_DNS,
    IOTK_P2P_ERROR_SOCKET,
    IOTK_P2P_ERROR_CALLER_RELAY,
    IOTK_P2P_ERROR_CALLER_STUN,
    IOTK_P2P_ERROR_CALLEE_STUN,
    IOTK_P2P_ERROR_CALLEE_WAIT_CALLER_CHECKNET_TIMEOUT,
    IOTK_P2P_ERROR_PEER_TIMEOUT,
    IOTK_P2P_ERROR_USER_CANCEL,
    IOTK_P2P_ERROR_CONNECTION_CHECK,
    IOTK_P2P_ERROR_CHANNEL,
    IOTK_P2P_ERROR_UNKNOWN11,
    IOTK_P2P_ERROR_UNKNOWN12,
    IOTK_P2P_ERROR_RTCP_TIMEOUT,
    /* Server initiate error */
    IOTK_P2P_ERROR_SERVER_ERROR_START = 100,
    IOTK_P2P_CAUSE_PEERNOTEXIST = IOTK_P2P_ERROR_SERVER_ERROR_START,
    IOTK_P2P_CAUSE_PEERDISCONNECT,
    IOTK_P2P_CAUSE_PEERINCONNECT,
    IOTK_P2P_CAUSE_CALLER_NOTLOGIN
};

enum IOTK_P2P_EVENT_ID
{
    IOTK_P2P_EVENT_SDP_UPDATE,
    IOTK_P2P_EVENT_CALL_ERROR,
};

struct IOTK_GUID;
struct timeval;
struct sockaddr_in;

struct IOTK_P2P_EVENT
{
    enum IOTK_P2P_EVENT_ID id;
    union
    {
        enum IOTK_P2P_CALL_ERROR call_error;
        struct P2P_SDP *sdp_update;
    } u;
};

typedef void (*IOTK_P2P_GET_TIMESTAMP_CALLBACK)(struct timeval *);
typedef void (*IOTK_P2P_EVENT_CALLBACK)(struct IOTK_P2P_EVENT *);
typedef void (*IOTK_P2P_DATA_INCOMING_CALLBACK)(const void *buf, unsigned int len);
typedef int (*IOTK_P2P_DATA_OUTGOING_CALLBACK)(int (*p2p_send)(const char *buf, unsigned int len));

enum IOTK_P2P_OPT
{
    IOTK_P2P_OPT_GET_TIMESTAMP_CALLBACK,
    IOTK_P2P_OPT_EVENT_CALLBACK,
    IOTK_P2P_OPT_DATA_OUTGOING_CALLBACK,
    IOTK_P2P_OPT_DATA_INCOMING_CALLBACK,
};

int iotk_p2p_setopt(enum IOTK_P2P_OPT opt, ...);

int iotk_p2p_stop(void);

int iotk_p2p_start_unicast(struct sockaddr_in *peer);

int iotk_p2p_runloop(void);

//void *iotk_p2p_query_interface(const struct IOTK_GUID *iid);

int iotk_p2p_update_peer_addr(struct P2P_SDP *sdp);

int iotk_p2p_accept_call(unsigned short channel, struct P2P_PEER *peers);

int iotk_p2p_init(void);

#endif // __IOTK_P2P_H__
