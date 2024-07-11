#ifndef conn_h
#define conn_h
#include "lwip/altcp.h"

extern altcp_allocator_t allocator;

// connection state enum
enum _conn_state
{
    CONN_NONE,
    CONN_AWAIT_DHCP,
    CONN_AWAIT_DNS_LOOKUP,
    CONN_TCP_CONNECT,
    CONN_TCP_CONNECTED
};

enum _client_state
{
    CLIENT_DISCONNECTED,
    CLIENT_CONNECTED,
    CLIENT_AWAITING_AUTH,
    CLIENT_AUTHORIZED,
    CLIENT_AWAITING_ASSETS,
    CLIENT_AWAITING_SERVER_JOIN,
    CLIENT_PLAYING,
    CLIENT_AWAITING_DISCONNECT_ACK
};

struct _conn
{
    struct netif *ethif;
    ip_addr_t addr;
    uint16_t port;
    struct altcp_pcb *pcb;
    uint24_t state;
    uint24_t error; // may not use
};

struct _client
{
    uint24_t state;
};

err_t altcp_connected_callback(void *arg, struct altcp_pcb *tpcb, err_t err);
err_t altcp_err_callback(void *arg, struct altcp_pcb *tpcb, err_t err);

#endif