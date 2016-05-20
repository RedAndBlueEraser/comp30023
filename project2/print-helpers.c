/*
 * print-helpers.c
 * Version 20160514
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include "print-helpers.h"

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void fprint_current_time(FILE *stream)
{
    time_t current_time;
    struct tm *current_time_as_struct_tm;

    /* Get current time. */
    current_time = time(NULL);
    if (current_time == (time_t)(-1))
    {
        perror("time");
        exit(1);
    }

    /* Print time. */
    current_time_as_struct_tm = localtime(&current_time);
    fprintf(
        stream,
        "[%.2d %.2d %.4d %.2d:%.2d:%.2d]",
        current_time_as_struct_tm->tm_mday,
        current_time_as_struct_tm->tm_mon + 1,
        current_time_as_struct_tm->tm_year + 1900,
        current_time_as_struct_tm->tm_hour,
        current_time_as_struct_tm->tm_min,
        current_time_as_struct_tm->tm_sec
        );

    return;
}

void print_current_time()
{
    fprint_current_time(stdout);
    return;
}

void fprint_ip_address(FILE *stream, struct sockaddr_in address)
{
    unsigned long ip_address_as_ulong = address.sin_addr.s_addr;
    unsigned char octets[4];
    octets[0] = ip_address_as_ulong & 0xFF;
    octets[1] = (ip_address_as_ulong >> 8) & 0xFF;
    octets[2] = (ip_address_as_ulong >> 16) & 0xFF;
    octets[3] = (ip_address_as_ulong >> 24) & 0xFF;	
    fprintf(stream, "(%d.%d.%d.%d)", octets[0], octets[1], octets[2], octets[3]);
    return;
}

void print_ip_address(struct sockaddr_in address)
{
    fprint_ip_address(stdout, address);
    return;
}

void fprint_current_time_and_ip_address(FILE *stream, struct sockaddr_in address)
{
    fprint_current_time(stream);
    fprint_ip_address(stream, address);
    return;
}

void print_current_time_and_ip_address(struct sockaddr_in address)
{
    fprint_current_time_and_ip_address(stdout, address);
    return;
}
