#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"

#include "conn.h"
#include "splash.h"
#include "gamestate.h"

altcp_allocator_t allocator = {altcp_tcp_alloc, NULL};

err_t altcp_recv_callback(void *arg, struct altcp_pcb *tpcb, err_t err)
{
}

err_t altcp_sent_callback(void *arg, struct altcp_pcb *tpcb, err_t err)
{
}

err_t altcp_connected_callback(void *arg, struct altcp_pcb *tpcb, err_t err)
{
    struct _gamestate *gamestate = (struct _gamestate *)arg;
    gamestate->conn.state = CONN_TCP_CONNECTED;
    splash_ConsolePrintLine("altcp_connect: connected, initing client");
    gamestate->client.state = CLIENT_CONNECTED;
    altcp_recv(tpcb, altcp_recv_callback);
    altcp_sent(tpcb, altcp_sent_callback);
}

err_t altcp_err_callback(void *arg, struct altcp_pcb *tpcb, err_t err)
{
    splash_ConsolePrintLine("altcp: an error occured");
}