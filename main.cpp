/*
 * Copyright 2017, Helium Systems, Inc.
 * All Rights Reserved. See LICENSE.txt for license information
 */

#include "mbed.h"
#include "Helium.h"
#include <inttypes.h>

void report_status(int status)
{
    if (helium_status_OK == status) {
        printf("Succeeded\n");
    } else {
        printf("Failed (status %d)\n", status);
    }
}

void report_status_result(int status, int result)
{
    if (helium_status_OK == status) {
        if (result == 0) {
            printf("Succeeded\n");
        } else {
            printf("Failed - %d", result);
        }
    } else {
        printf("Failed (status %d)\n", status);
    }
}


#if defined(TARGET_K64F)
Helium helium(D9, D7); // TX:D9, RX:D7
#elif defined(TARGET_WIZwiki_W7500)
Helium helium(D1, D0);
#else
#error Please define helium with serial tx/rx
#endif

Channel channel(&helium);

int main()
{
    printf("Starting\n");

    printf("Info - ");
    struct helium_info info;
    int status = helium.info(&info);
    if (helium_status_OK == status) {
        printf("%" PRIx64 " - ", info.mac);
    }
    report_status(status);

    printf("Connecting - ");
    status = helium.connect(NULL, HELIUM_POLL_RETRIES_5S * 10);
    report_status(status);

    printf("Creating Channel - ");
    int8_t result;
    status = channel.begin("Helium Cloud MQTT", &result);
    report_status(status);

    printf("Sending - ");
    const char *data = "Hello Helium";
    status = channel.send(data, strlen(data), &result);
    report_status_result(status, result);
}
