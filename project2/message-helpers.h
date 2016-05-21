/*
 * message-helpers.h
 * Version 20160521
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define READ_ATTEMPTS_MAX_COUNT  5
#define WRITE_ATTEMPTS_MAX_COUNT 5

#define READ_ERROR            -1
#define WRITE_CONN_LOST_ERROR -2
#define WRITE_OTHER_ERROR     -1

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Send the specified message to the specified socket file descriptor and
 * returns msg_size on success or < 0 on error. The sent message is stored in
 * buf.
 */
int receive_message(int socket_fd, char buf[], int msg_size);
/* Receive a message from the specified socket file descriptor and returns
 * msg_size on success or -1 on error. The received message is stored in buf.
 */
int send_message(int socket_fd, char msg[], int msg_size);
