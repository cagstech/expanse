
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

// connection initial configuration
const altcp_allocator_t allocator = {altcp_tcp_alloc, NULL};
const char *remote_host = "remote.titrek.us";
const uint16_t remote_port = 51701;

// netstate
uint16_t struct _netstate
{
    struct netif *ethif;
    ip_addr_t conn_addr;
    struct altcp_pcb *conn;
};
struct _netstate netstate;

void dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
}

void ethif_status_callback_fn(struct netif *netif)
{
    if (!dhcp_supplied_address(netif))
        dhcp_start();
    else if (dhcp_supplied_address(netif))
    {
    }
}

int main(void)
{
    lwip_init();
    gfx_Begin();
    gfx_FillScreen(0);

    // initialize altcp pcb
    netstate.conn = altcp_new(&allocator);

    /* You should probably handle this function failing */
    if (usb_Init(eth_handle_usb_event, NULL, NULL, USB_DEFAULT_INIT_FLAGS))
        goto exit;

    run_main = true;

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
        if (ethif == NULL)
        {
            if ((ethif = netif_find("en0")))
            {
                // run this code if netif exists
                // eg: dhcp_start(ethif);
                printf("en0 registered\n");
                netif_set_status_callback(ethif, ethif_status_callback_fn);
            }
        }
        usb_HandleEvents();   // usb events
        sys_check_timeouts(); // lwIP timers/event callbacks
    } while (run_main);
    dhcp_release_and_stop(ethif);
exit:
    usb_Cleanup();
    exit(0);
}
