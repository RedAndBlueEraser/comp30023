/*
 * server.c
 * Version 20160520
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <pthread.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include "game-protocol.h"
#include "print-helpers.h"
#include "secret-code.h"

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define LOG_FILE_FILENAME "log.txt"
#define CLIENTS_MAX_LEN   20

#define BUFFER_SIZE       512
#define BUFFER_LEN        BUFFER_SIZE - 1
#define MAX_GUESSES_COUNT 10

////////////////////////////////////////////////////////////////////////////////
// Data structures. ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Data structure to hold client connection information. */
typedef struct client_t
{
    struct sockaddr_in address;
    unsigned int address_size;
    int new_sock_fd;
} client_t;

/* Data structure to hold arguments passed to pthread_create. */
typedef struct pthread_arg_t
{
    char secret_code[SECRET_CODE_LEN + 1];
    client_t client;
} pthread_arg_t;

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Signal handler. */
void sig_handler(int sig_number);
/* Thread runner. */
void *pthread_routine(void *param);
/* Send the specified message to the specified socket file descriptor and
 * returns the number of bytes sent. The raw sent message is stored in buf.
 */
int send_message(int new_sock_fd, char buf[], int buf_size, char *msg_type, const char *payload);
/* Receive a message from the specified socket file descriptor and returns the
 * the number of bytes received. The received message is stored in buf.
 */
int receive_message(int new_sock_fd, char buf[], int buf_size);

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
FILE *log_file_fd;
int socket_fd;
struct sockaddr_in server_address;
int clients_count = 0, clients_win_count = 0;
pthread_mutex_t clients_win_count_mutex;

void sig_handler(int sig_number)
{
    struct rusage usage;

    if (sig_number == SIGINT)
    {
        // Close socket.
        close(socket_fd);

        // Log server shutdown.
        fprint_current_time_and_ip_address(log_file_fd, server_address);
        fprintf(log_file_fd, " server shutdown\n");

        fprintf(log_file_fd, "%d clients connected during session\n", clients_count);
        fprintf(log_file_fd, "%d clients successfully guessed the secret code\n", clients_win_count);

        getrusage(RUSAGE_SELF, &usage);
        fprintf(
            log_file_fd,
            "%ld.%06lds CPU time spent in executing in user mode\n",
            usage.ru_utime.tv_sec,
            usage.ru_utime.tv_usec
            );
        fprintf(
            log_file_fd,
            "%ld.%06lds CPU time spent in executing in kernel mode\n",
            usage.ru_stime.tv_sec,
            usage.ru_stime.tv_usec
            );

        fflush(log_file_fd);

        // Close file.
        fclose(log_file_fd);

        exit(0);
    }
    return;
}

int main(int argc, char *argv[])
{
    int port;
    char secret_code[SECRET_CODE_LEN + 1];
    pthread_arg_t *pthread_arg;
    client_t *client;
    pthread_t pthread;

    // Not enough program arguments.
    if (argc < 2)
    {
        fprintf(stderr, "Unspecified port number\n");
        exit(1);
    }

    // Get program arguments.
    port = atoi(argv[1]);
    if (argc >= 3 && is_valid_secret_code(argv[2]))
    {
        strncpy(secret_code, argv[2], SECRET_CODE_LEN + 1);
    }
    else
    {
        memset(secret_code, 0, SECRET_CODE_LEN + 1);
    }

    // Open file to write log.
    log_file_fd = fopen(LOG_FILE_FILENAME, "w");
    if (log_file_fd == NULL)
    {
        perror("fopen");
        exit(1);
    }

    // Set signal handler function.
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    // Create TCP socket.
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    // Initialise server address.
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind server address to socket.
    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind");
        exit(1);
    }

    // Listen on socket.
    if (listen(socket_fd, CLIENTS_MAX_LEN) == -1)
    {
        perror("listen");
        exit(1);
    }
    fprint_current_time_and_ip_address(log_file_fd, server_address);
    fprintf(log_file_fd, " server started\n");
    fflush(log_file_fd);

    // Accept and handle connections.
    while (1)
    {
        /* Store secret code and client connection information in an argument
         * object to be passed to pthread_create().
         */
        pthread_arg = (pthread_arg_t*)malloc(sizeof(pthread_arg_t));
        if (pthread_arg == NULL)
        {
            perror("malloc");
            continue;
        }
        strncpy(pthread_arg->secret_code, secret_code, SECRET_CODE_LEN + 1);
        client = &(pthread_arg->client);
        client->address_size = sizeof(client->address);
        // Accept connection.
        client->new_sock_fd = accept(socket_fd, (struct sockaddr *) &(client->address), &(client->address_size));
        if (client->new_sock_fd == -1)
        {
            perror("accept");
            free(pthread_arg);
            continue;
        }

        // Log client connect.
        fprint_current_time_and_ip_address(log_file_fd, client->address);
        fprintf(log_file_fd, "(%d) client connected\n", client->new_sock_fd);
        fflush(log_file_fd);

        clients_count++;

        // Create thread to serve client.
        if (pthread_create(&pthread, NULL, pthread_routine, (void*)pthread_arg) != 0)
        {
            perror("pthread_create");

            fprint_current_time_and_ip_address(log_file_fd, server_address);
            fprintf(log_file_fd, " server failed to create thread for ");
            fprint_ip_address(log_file_fd, client->address);
            fprintf(log_file_fd, "(%d)\n", client->new_sock_fd);
            fflush(log_file_fd);

            close(client->new_sock_fd);

            free(pthread_arg);
            continue;
        }
    }

    // Close socket in signal handler.
    // Close file in signal handler.

    return 0;
}

void *pthread_routine(void *param)
{
    pthread_arg_t *pthread_param;
    char secret_code[SECRET_CODE_LEN + 1];
    // TODO: struct sockaddr_in client_address;
    //unsigned int client_address_size;
    int new_sock_fd;

    const char WELCOME_MESSAGE[BUFFER_SIZE - GP_HEADER_LEN] =
        "\n"
        "Welcome to online Mastermind game.\n"
        "\n"
        "Instructions: Win the game by correctly guessing the secret code within ten\n"
        "guesses. The code consists of four uppercase characters A, B, C, D, E and F.\n"
        "Enter the guess when prompted with \">\". Do not include any spaces, lowercase\n"
        "letters or any other characters.\n"
        "\n";
    char buffer[BUFFER_SIZE];
    int guesses_count = 0; // TODO: , correct_colors_count, correct_positions_count;

    // Load parameters and free memory space allocated to hold param.
    pthread_param = (pthread_arg_t*)param;
    strncpy(secret_code, pthread_param->secret_code, SECRET_CODE_LEN + 1);
    // TODO: client_address = pthread_param->client.address;
    //client_address_size = pthread_param->client.address_size;
    new_sock_fd = pthread_param->client.new_sock_fd;
    free(param);

    // Generate secret code if none provided.
    if (!is_valid_secret_code(secret_code))
    {
        get_random_secret_code(secret_code);
    }

    // Log server secret code.
    fprint_current_time(log_file_fd);
    fprint_ip_address(log_file_fd, server_address);
    fprintf(log_file_fd, " server's secret = %s\n", secret_code);
    fflush(log_file_fd);

    /* Begin processing with client. */

    // Message welcome message to client.
    send_message(new_sock_fd, buffer, BUFFER_SIZE, GP_MESSAGE, WELCOME_MESSAGE);

    // While game has not ended.
    while (guesses_count < MAX_GUESSES_COUNT) // TODO: && correct_positions_count < SECRET_CODE_LEN)
    {
        // TODO: correct_colors_count = 0;
        // TODO: correct_positions_count = 0;

        // Request guess from client.
        send_message(new_sock_fd, buffer, BUFFER_SIZE, GP_GUESS_REQ, NULL);

        /* If client enters invalid guess, repeat until client submits valid
         * guess.
         */
        while (1)
        {
            // Read guess sent from client.
            receive_message(new_sock_fd, buffer, BUFFER_SIZE);

            // Guess is valid.
            if (is_valid_secret_code(buffer))
                break;
            // Guess is invalid, inform of invalid guess to client.
            else
                send_message(new_sock_fd, buffer, BUFFER_SIZE, GP_GUESS_INV, NULL);
        }

        guesses_count++;
    }

    return NULL;
}

// TODO:
#include <assert.h>
int send_message(int new_sock_fd, char buf[], int buf_size, char *msg_type, const char *payload)
{
    int i;

    if (payload == NULL) payload = "";
    memset(buf, 0, buf_size);
    sprintf(buf, "%s%s", msg_type, payload);
    fprintf(stderr, "buf=%s strlen(buf)+1=%ld ", buf, strlen(buf)+1); // TODO:
    i = write(new_sock_fd, buf, strlen(buf) + 1);
    fprintf(stderr, "i=%d\n", i); // TODO:
    assert(i == strlen(buf) + 1); // TODO:
    return i;
}

int receive_message(int new_sock_fd, char buf[], int buf_size)
{
    memset(buf, 0, buf_size);
    return read(new_sock_fd, buf, buf_size);
}
