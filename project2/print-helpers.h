/*
 * print-helpers.h
 * Version 20160514
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Print the current time to the specified file stream. */
void fprint_current_time(FILE *stream);
/* Print the current time. */
void print_current_time();
/* Print the specified address to the specified file stream. */
void fprint_ip_address(FILE *stream, struct sockaddr_in address);
/* Print the specified address. */
void print_ip_address(struct sockaddr_in address);
/* Print the current time and the specified address to the specified file
 * stream.
 */
void fprint_current_time_and_ip_address(FILE *stream, struct sockaddr_in address);
/* Print the current time and the specified address. */
void print_current_time_and_ip_address(struct sockaddr_in address);
