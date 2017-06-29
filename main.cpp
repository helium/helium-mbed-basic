/*
 * Copyright 2017, Helium Systems, Inc.
 * All Rights Reserved. See LICENCE.txt for license information
 */

#include "mbed.h"
#include "Helium.h"

Helium helium(PTC17, PTC16);
Channel channel(&helium);

void report_status(int status)
{
    if (helium_status_OK == status) {
        printf("Succeeded\n");
    } else {
        printf("Failed\n");
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
        printf("Failed\n");
    }
}


int main()
{
    int status = helium.connect();

    int8_t result;
    status = channel.begin("Helium Cloud MQTT", &result);
    report_status(status);

    const char *data = "Hello Helium";

    status = channel.send(data, strlen(data), &result);
    report_status_result(status, result);
}
