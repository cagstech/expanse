#include <string.h>

#include <usbdrvce.h>
#include <ti/getcsc.h>
#include <ti/screen.h>

#include "drivers/usb-ethernet.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"

/* These commented out, but may be headers you might wish to enable. */
#include "lwip/altcp_tcp.h"
#include "lwip/altcp.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"

#include "engines/conn.h"

// connection initial configuration

#define HOSTNAME_MAX_LEN 128
#define DEFAULT_PORT 51701

// gamestate
struct _gamestate
{
    struct _conn conn;
};
struct _gamestate gamestate = {0};

void dns_lookup_recv(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    memcpy(&gamestate.conn.addr, ipaddr, sizeof(ip_addr_t));
    if (altcp_connect(gamestate.conn.pcb, &gamestate.conn.addr, gamestate.conn.port, altcp_connected_callback))
        printf("internal error, altcp_connect\n");
    gamestate.conn.state = CONN_TCP_CONNECT;
}

void ethif_status_callback_fn(struct netif *netif)
{
    switch (gamestate.conn.state)
    {
    case CONN_NONE:
        dhcp_start(netif);
        printf("starting dhcp_client\n");
        gamestate.conn.state = CONN_AWAIT_DHCP;
        break;
    case CONN_AWAIT_DHCP:
        if (dhcp_supplied_address(netif))
        {
            char hostname[HOSTNAME_MAX_LEN];
            size_t s = 0;
            printf("dhcp config complete\n");
            os_GetStringInput("connect to: ", hostname, HOSTNAME_MAX_LEN);
            printf("attempting dns lookup for %s\n", hostname);
            if (ipaddr_aton(hostname, &gamestate.conn.addr))
                goto altcp_start;
            for (s = 0; s < strlen(hostname); s++)
                if (hostname[s] == ':')
                    break;
            if (s == strlen(hostname))
            {
                printf("no port specified, using default\n");
                gamestate.conn.port = DEFAULT_PORT;
            }
            else if (s < strlen(hostname))
            {
                hostname[s++] = 0;
                for (; s < strlen(hostname); s++)
                {
                    if (hostname[s])
                    {
                        gamestate.conn.port *= 10;
                        gamestate.conn.port += (hostname[s] - '0');
                    }
                    else
                        break;
                }
                printf("port %u specified\n", gamestate.conn.port);
                err_t dns_err = dns_gethostbyname(hostname, &gamestate.conn.addr, dns_lookup_recv, NULL);
                if (dns_err == ERR_OK)
                    printf("dns in cache\n");
                else if (dns_err == ERR_ARG)
                    printf("dns argument invalid\n");
                gamestate.conn.state = CONN_TCP_CONNECT;
            }
        altcp_start:
            if (altcp_connect(gamestate.conn.pcb, &gamestate.conn.addr, gamestate.conn.port, altcp_connected_callback))
                printf("internal error, altcp_connect\n");
            break;
        }
        break;
    }
}

int main(void)
{
    lwip_init();
    os_ClrLCDFull();
    os_HomeUp();

    // initialize altcp pcb
    gamestate.conn.pcb = altcp_new(&allocator);
    altcp_arg(gamestate.conn.pcb, &gamestate);
    altcp_err(gamestate.conn.pcb, altcp_err_callback);

    /* You should probably handle this function failing */
    if (usb_Init(eth_handle_usb_event, NULL, NULL, USB_DEFAULT_INIT_FLAGS))
        goto exit;

    bool run_main = true;
    sk_key_t key;

    do
    {
        // this is your code that runs in a loop
        // please note that much of the networking in lwIP is callback-style
        // please consult the lwIP documentation for the protocol you are using for instructions
        key = os_GetCSC();
        if (key == sk_Clear)
        {
            run_main = false;
        }
        if (gamestate.conn.ethif == NULL)
        {
            if ((gamestate.conn.ethif = netif_find("en0")))
            {
                // run this code if netif exists
                // eg: dhcp_start(ethif);
                printf("if en0 registered\n");
                netif_set_default(gamestate.conn.ethif);
                netif_set_status_callback(gamestate.conn.ethif, ethif_status_callback_fn);
            }
        }
        usb_HandleEvents();   // usb events
        sys_check_timeouts(); // lwIP timers/event callbacks
    } while (run_main);
    dhcp_release_and_stop(gamestate.conn.ethif);
exit:
    netif_remove(gamestate.conn.ethif);
#if ETH_DEBUG_FILE == LWIP_DBG_ON
    fclose(eth_logger);
#endif
    usb_Cleanup();
    // gfx_End();
    exit(0);
}
