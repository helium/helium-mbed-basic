/*
 * Copyright 2017, Helium Systems, Inc.
 * All Rights Reserved. See LICENSE.txt for license information
 */


#include "mbed.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "Helium.h"
#include "HeliumUtil.h"

#define CHANNEL_NAME "Helium MQTT"
#define CONFIG_INTERVAL_KEY "config.interval_ms"
#define DEFAULT_INTERVAL 500

#if defined(TARGET_K64F)
Helium helium(D9, D7); // TX:D9, RX:D7
#elif defined(TARGET_WIZwiki_W7500)
Helium helium(D1, D0);
#else
#error Please define helium with serial tx/rx
#endif

Channel channel(&helium);
Config config(&channel);
int32_t send_interval;

void
update_config(bool stale)
{
    if (stale)
    {
        DBG_PRINTF("Fetching Config - ");
        int status = config.get(CONFIG_INTERVAL_KEY, &send_interval, DEFAULT_INTERVAL);
        report_status(status);
    }
}

int main()
{
    DBG_PRINTF("Starting\n");

    // Let the Atom start up
    wait(0.1);

    // Get and print the mac address
    DBG_PRINTF("Info - ");
    struct helium_info info;
    int status = helium.info(&info);
    if (helium_status_OK == status) {
        DBG_PRINTF("%" PRIx64 " - ", info.mac);
    }
    report_status(status);

    // Tell the Atom to connect to the network. The used HeliumUtil
    // will keep trying to connect forever.
    helium_connect(&helium);

    // Create a channel just to get it done.. The HeliumUtil
    // implementation of this will both connect and retry channel
    // creation.
    channel_create(&channel, CHANNEL_NAME);

    // Fetch configuration data from channel.
    update_config(true);

    while(true) {
        // Send some data. This uses the HeliumUtil utility that both
        // re-connects to the channel, re-connects and/or re-sends
        // under failure conditions.
        const char *data = "Hello Helium";
        channel_send(&channel, CHANNEL_NAME, data, strlen(data));

        // Update configuration if the network tells us it's stale.
        update_config(config.is_stale());

        // Wait for the configured interval
        wait_ms(send_interval);
    }
}
