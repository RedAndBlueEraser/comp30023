/*
 * message-helpers.c
 * Version 20160521
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "message-helpers.h"

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int receive_message(int socket_fd, char buf[], int msg_size)
{
    int rcvd_msg_size = 0, n, read_attempts_count = 0;

    do
    {
        n = read(socket_fd, buf + rcvd_msg_size, msg_size - rcvd_msg_size);

        if (n > 0)
        {
            rcvd_msg_size += n;
        }

    } while (rcvd_msg_size < msg_size && ++read_attempts_count < READ_ATTEMPTS_MAX_COUNT);

    // If the entire message was not received, return with error.
    if (rcvd_msg_size != msg_size)
    {
        perror("read");
        rcvd_msg_size = READ_ERROR;
    }

    return rcvd_msg_size;
}

int send_message(int socket_fd, char msg[], int msg_size)
{
    int sent_msg_size = 0, n, write_attempts_count = 0;

    do
    {
        n = write(socket_fd, msg + sent_msg_size, msg_size - sent_msg_size);

        // If the client is not receiving, return with error.
        if (n == -1 && errno == EPIPE)
        {
            perror("write");
            sent_msg_size = WRITE_CONN_LOST_ERROR;
            break;
        }
        else if (n > 0)
        {
            sent_msg_size += n;
        }

    } while (sent_msg_size < msg_size && ++write_attempts_count <  WRITE_ATTEMPTS_MAX_COUNT);

    // If the entire message was not sent, return with error.
    if (sent_msg_size != msg_size && sent_msg_size != WRITE_CONN_LOST_ERROR)
    {
        perror("write");
        sent_msg_size = WRITE_OTHER_ERROR;
    }

    return sent_msg_size;
}
