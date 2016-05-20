/*
 * client.c
 * Version 20160520
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <netdb.h> 
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "game-protocol.h"

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    struct hostent *server_hostname;
    int server_port;
    struct sockaddr_in server_address;
    int socket_fd;

    // Not enough program arguments.
    if (argc < 3)
    {
        fprintf(stderr, "Unspecified hostname/IP address or port number\n");
        exit(1);
    }

    // Get program arguments.
    server_hostname = gethostbyname(argv[1]);
    if (server_hostname == NULL)
    {
        fprintf(stderr, "No such host\n");
        exit(1);
    }
    server_port = atoi(argv[2]);

    // Initialise server address.
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy(
        (char*)server_hostname->h_addr,
        (char*)&server_address.sin_addr.s_addr,
        server_hostname->h_length
        );
    server_address.sin_port = htons(server_port);

    // Create TCP socket.
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    // Connect to server address with TCP socket.
    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		perror("connect");
        exit(1);
	}

    const int BUFFER_LEN = 511,
        GAME_PROTOCOL_HEADER_LEN = 7;
    const char
        MESSAGE          [8] = "MESSAGE",
        GUEST_REQUEST    [8] = "REQUEST",
        GUESS_FEEDBACK   [8] = "FEEDBAC",
        GUESS_INVALID    [8] = "INVALID",
        GAME_SUCCESS     [8] = "SUCCESS",
        GAME_FAILURE     [8] = "FAILURE";

    char buffer[511 + 1] = "", c;
    int is_end_game = 0, guesses_count = 0, correct_colors_count = 0,
        correct_positions_count = 0;

    while (!is_end_game)
    {
        read(socket_fd, buffer, sizeof(buffer));

        if (strncmp(buffer, MESSAGE, GAME_PROTOCOL_HEADER_LEN) == 0)
        {
            printf("%s", &(buffer[GAME_PROTOCOL_HEADER_LEN]));
        }
        else if (strncmp(buffer, GUEST_REQUEST, GAME_PROTOCOL_HEADER_LEN) == 0)
        {
            printf(">");
            fgets(buffer, BUFFER_LEN + 1, stdin);
            buffer[strcspn(buffer, "\r\n")] = 0;
            write(socket_fd, buffer, strlen(buffer));
        }
        else if (strncmp(buffer, GUESS_INVALID, GAME_PROTOCOL_HEADER_LEN) == 0)
        {
            printf("Invalid guess.\n>");
            fgets(buffer, BUFFER_LEN + 1, stdin);
            buffer[strcspn(buffer, "\r\n")] = 0;
            write(socket_fd, buffer, strlen(buffer));
        }

        memset(buffer, 0, sizeof(buffer));
    }

    // Close socket.
    close(socket_fd);

    return 0;
}
