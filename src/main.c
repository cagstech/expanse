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

#include "components/helpers/conn.h"
#include "components/helpers/splash.h"
#include "components/helpers/gamestate.h"
#include "components/asm/asm.h"

// connection initial configuration

#define HOSTNAME_MAX_LEN 64
#define DEFAULT_PORT 51701

// gamestate

struct _gamestate gamestate = {0};

void dns_lookup_recv(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    char sprintf_output_string[HOSTNAME_MAX_LEN * 2];
    memcpy(&gamestate.conn.addr, ipaddr, sizeof(ip_addr_t));
    sprintf(sprintf_output_string, "altcp_connect ip=%s port=%u",
            ipaddr_ntoa(&gamestate.conn.addr),
            gamestate.conn.port);
    splash_ConsolePrintLine(sprintf_output_string);
    if (altcp_connect(gamestate.conn.pcb, &gamestate.conn.addr, gamestate.conn.port, altcp_connected_callback))
        splash_ConsolePrintLine("altcp_connect fatal error");
    gamestate.conn.state = CONN_TCP_CONNECT;
}

void ethif_status_callback_fn(struct netif *netif)
{
    switch (gamestate.conn.state)
    {
    case CONN_NONE:
        dhcp_start(netif);
        splash_ConsolePrintLine("initiating dhcp_client");
        gamestate.conn.state = CONN_AWAIT_DHCP;
        break;
    case CONN_AWAIT_DHCP:
        if (dhcp_supplied_address(netif))
        {
            char hostname[HOSTNAME_MAX_LEN];
            size_t s = 0;
            char sprintf_output_string[HOSTNAME_MAX_LEN * 2];
            sprintf(sprintf_output_string, "dhcp address=%s", ipaddr_ntoa(netif_ip4_addr(netif)));
            splash_ConsolePrintLine(sprintf_output_string);
            splash_ConsoleClearLine();
#define HOSTNAME_INPUT_X 10
#define HOSTNAME_INPUT_Y 210 + 12
            gfx_SetTextXY(HOSTNAME_INPUT_X, HOSTNAME_INPUT_Y);
            user_input(hostname, HOSTNAME_MAX_LEN, 0);
            splash_ConsolePrintLine("hostname input done");
            if (ipaddr_aton(hostname, &gamestate.conn.addr))
                goto altcp_start;
            for (s = 0; s < strlen(hostname); s++)
                if (hostname[s] == ':')
                    break;
            if (s == strlen(hostname))
                gamestate.conn.port = DEFAULT_PORT;
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
                sprintf(sprintf_output_string, "dns_lookup host=%s", hostname);
                splash_ConsolePrintLine(sprintf_output_string);
                err_t dns_err = dns_gethostbyname(hostname, &gamestate.conn.addr, dns_lookup_recv, NULL);
                if (dns_err == ERR_OK)
                    goto altcp_start;
                else if (dns_err == ERR_INPROGRESS)
                    break;
                else
                {
                    splash_ConsolePrintLine("dns_gethostbyname fatal error");
                    break;
                }
                gamestate.conn.state = CONN_TCP_CONNECT;
            }
        altcp_start:
            sprintf(sprintf_output_string, "altcp_connect ip=%s port=%u",
                    ipaddr_ntoa(&gamestate.conn.addr),
                    gamestate.conn.port);
            splash_ConsolePrintLine(sprintf_output_string);
            if (altcp_connect(gamestate.conn.pcb, &gamestate.conn.addr, gamestate.conn.port, altcp_connected_callback))
                splash_ConsolePrintLine("altcp_connect fatal error");
            break;
        }
        break;
    }
}

int main(void)
{
    lwip_init();
    gfx_Begin();
    gfx_SetTextBGColor(1);
    gfx_SetTextTransparentColor(1);
    gfx_SetDrawBuffer();

    gfx_SetTextFGColor(191);
    splash_RenderStarfield();

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
                splash_ConsolePrintLine("network if registered");
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
    gfx_End();
    exit(0);
}
