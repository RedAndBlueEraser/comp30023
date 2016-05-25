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
#include "message-helpers.h"

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int receive_message(int socket_fd, char buf[], int buf_size);

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    struct hostent *server_hostname;
    int server_port;
    struct sockaddr_in server_address;
    int socket_fd;

    char buffer[GP_SIZE] = "";
    int is_end_game = 0;

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

    /******************** Begin communication with server. ********************/

    while (!is_end_game)
    {
        receive_message(socket_fd, buffer, GP_SIZE);

        if (strncmp(buffer, GP_DISP_MSG, GP_HEADER_SIZE) == 0)
        {
            printf("%s", &(buffer[GP_HEADER_SIZE]));
        }
        else if (strncmp(buffer, GP_GUESS_REQ, GP_HEADER_SIZE) == 0)
        {
            printf("> ");
            fgets(buffer, GP_SIZE, stdin);
            buffer[strcspn(buffer, "\r\n")] = 0;
            send_message(socket_fd, buffer, GP_SIZE);
        }
        else if (strncmp(buffer, GP_GUESS_INV, GP_HEADER_SIZE) == 0)
        {
            printf("Invalid guess.\n> ");
            fgets(buffer, GP_SIZE, stdin);
            buffer[strcspn(buffer, "\r\n")] = 0;
            send_message(socket_fd, buffer, GP_SIZE);
        }
        else if (strncmp(buffer, GP_GUESS_FBK, GP_HEADER_SIZE) == 0)
        {
            printf("%s", &(buffer[GP_HEADER_SIZE]));
        }
        else if (strncmp(buffer, GP_GAME_SUCC, GP_HEADER_SIZE) == 0)
        {
            printf("You win! =)\n");
            is_end_game = 1;
        }
        else if (strncmp(buffer, GP_GAME_FAIL, GP_HEADER_SIZE) == 0)
        {
            printf("You lose. Better luck next time.\n");
            is_end_game = 1;
        }
        else if (strncmp(buffer, GP_SERV_FULL, GP_HEADER_SIZE) == 0)
        {
            printf("Server is full. Try connecting later.\n");
            is_end_game = 1;
        }

        memset(buffer, 0, sizeof(buffer));
    }

    // Close socket.
    close(socket_fd);

    return 0;
}
