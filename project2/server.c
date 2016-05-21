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
#include "message-helpers.h"
#include "print-helpers.h"
#include "secret-code.h"

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define LOG_FILE_FILENAME "log.txt"
#define CLIENTS_MAX_LEN   20

#define MAX_GUESSES_COUNT 10

////////////////////////////////////////////////////////////////////////////////
// Data structures. ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Data structure to hold client connection information. */
typedef struct client_t
{
    struct sockaddr_in address;
    unsigned int address_size;
    int new_socket_fd;
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
/* Increment the clients counter. */
void increment_clients_count();
/* Increment the clients win counter. */
void increment_clients_win_count();
/* Send the specified message encapsulated in the game protocol to the specified
 * client. The sent raw message is stored in buf.
 */
int send_gp_message(client_t *client, char buf[], char *msg_type, char *msg_payload);
/* Receive a message from the specified client and returns. The received message
 * is stored in buf.
 */
int receive_raw_message(client_t *client, char buf[]);
/* Thread runner. */
void *pthread_routine(void *param);

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
FILE *log_file_fd;
int socket_fd;
struct sockaddr_in server_address;
int clients_count = 0, clients_win_count = 0;
pthread_mutex_t log_file_mutex, clients_count_mutex, clients_win_count_mutex;

void sig_handler(int sig_number)
{
    struct rusage usage;

    // TODO: mutex and log file fd mutex and performance and report.
    if (sig_number == SIGINT)
    {
        // Close socket.
        close(socket_fd);

        // Log server shutdown.
        pthread_mutex_lock(&log_file_mutex);
        fprint_current_time_and_ip_address(log_file_fd, server_address);
        fprintf(log_file_fd, " server shutdown\n");
        fflush(log_file_fd);

        // Log client connection stats.
        fprintf(log_file_fd, "%d clients connected during session\n", clients_count);
        fprintf(log_file_fd, "%d clients successfully guessed the secret code\n", clients_win_count);
        fflush(log_file_fd);

        // Log server performance stats.
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

        // Destroy pthread mutexes.
        pthread_mutex_unlock(&log_file_mutex);
        if (pthread_mutex_destroy(&log_file_mutex) != 0)
        {
            perror("pthread_mutex_destroy");
        }
        if (pthread_mutex_destroy(&clients_count_mutex) != 0)
        {
            perror("pthread_mutex_destroy");
        }
        if (pthread_mutex_destroy(&clients_win_count_mutex) != 0)
        {
            perror("pthread_mutex_destroy");
        }

        exit(0);
    }
    return;
}

void increment_clients_count()
{
    pthread_mutex_lock(&clients_count_mutex);
    clients_count++;
    pthread_mutex_unlock(&clients_count_mutex);
    return;
}

void increment_clients_win_count()
{
    pthread_mutex_lock(&clients_win_count_mutex);
    clients_win_count++;
    pthread_mutex_unlock(&clients_win_count_mutex);
    return;
}

int send_gp_message(client_t *client, char buf[], char *msg_type, char *msg_payload)
{
    struct sockaddr_in client_address = client->address;
    int socket_fd = client->new_socket_fd;

    int sent_msg_size;

    /* Compose message and store in buffer, the payload only exists for
     * displaying messages and sending guess feedback to the client.
     */
    memset(buf, 0, GP_SIZE);
    if (msg_payload == NULL || (strcmp(msg_type, GP_DISP_MSG) != 0 && strcmp(msg_type, GP_GUESS_FBK) != 0))
    {
        sprintf(buf, "%s", msg_type);
    }
    else
    {
        sprintf(buf, "%s%s", msg_type, msg_payload);
    }

    // Send message.
    sent_msg_size = send_message(socket_fd, buf, GP_SIZE);

    if (sent_msg_size == WRITE_CONN_LOST_ERROR)
    {
        // Log client connection lost.
        pthread_mutex_lock(&log_file_mutex);
        fprint_current_time_and_ip_address(log_file_fd, client_address);
        fprintf(log_file_fd, "(%d) client connection unexpectedly closed\n", socket_fd);
        fflush(log_file_fd);
        pthread_mutex_unlock(&log_file_mutex);

        // Close socket.
        close(socket_fd);

        pthread_exit(NULL);
    }
    else if (sent_msg_size == WRITE_OTHER_ERROR)
    {
        // Log server fail to send.
        pthread_mutex_lock(&log_file_mutex);
        fprint_current_time_and_ip_address(log_file_fd, server_address);
        fprintf(log_file_fd, " server failed to send message to ");
        fprint_ip_address(log_file_fd, client_address);
        fprintf(log_file_fd, "(%d)\n", socket_fd);
        fflush(log_file_fd);

        // Close socket.
        close(socket_fd);

        // Log client disconnect.
        fprint_current_time_and_ip_address(log_file_fd, client_address);
        fprintf(log_file_fd, "(%d) client disconnected\n", socket_fd);
        fflush(log_file_fd);
        pthread_mutex_unlock(&log_file_mutex);

        pthread_exit(NULL);
    }

    return sent_msg_size;
}

int receive_raw_message(client_t *client, char buf[])
{
    struct sockaddr_in client_address = client->address;
    int socket_fd = client->new_socket_fd;

    int received_msg_size;

    memset(buf, 0, GP_SIZE);

    // Receive message.
    received_msg_size = receive_message(socket_fd, buf, GP_SIZE);

    if (received_msg_size == READ_ERROR)
    {
        // Log server fail to receive.
        pthread_mutex_lock(&log_file_mutex);
        fprint_current_time_and_ip_address(log_file_fd, server_address);
        fprintf(log_file_fd, " server failed to receive message from ");
        fprint_ip_address(log_file_fd, client_address);
        fprintf(log_file_fd, "(%d)\n", socket_fd);
        fflush(log_file_fd);

        // Close socket.
        close(socket_fd);

        // Log client disconnect.
        fprint_current_time_and_ip_address(log_file_fd, client_address);
        fprintf(log_file_fd, "(%d) client disconnected\n", socket_fd);
        fflush(log_file_fd);
        pthread_mutex_unlock(&log_file_mutex);

        pthread_exit(NULL);
    }

    return received_msg_size;
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

    // Initialise pthread mutex.
    if (pthread_mutex_init(&log_file_mutex, NULL) != 0)
    {
        perror("pthread_mutex_init");
        exit(1);
    }
    if (pthread_mutex_init(&clients_count_mutex, NULL) != 0)
    {
        perror("pthread_mutex_init");
        exit(1);
    }
    if (pthread_mutex_init(&clients_win_count_mutex, NULL) != 0)
    {
        perror("pthread_mutex_init");
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
    // Log server start.
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
        client->new_socket_fd = accept(socket_fd, (struct sockaddr *) &(client->address), &(client->address_size));
        if (client->new_socket_fd == -1)
        {
            perror("accept");
            free(pthread_arg);
            continue;
        }

        // Create thread to serve client.
        if (pthread_create(&pthread, NULL, pthread_routine, (void*)pthread_arg) != 0)
        {
            perror("pthread_create");

            // Log server fail to create thread.
            pthread_mutex_lock(&log_file_mutex);
            fprint_current_time_and_ip_address(log_file_fd, server_address);
            fprintf(log_file_fd, " server failed to create thread for ");
            fprint_ip_address(log_file_fd, client->address);
            fprintf(log_file_fd, "(%d)\n", client->new_socket_fd);
            fflush(log_file_fd);
            pthread_mutex_unlock(&log_file_mutex);

            close(client->new_socket_fd);

            free(pthread_arg);
            continue;
        }
    }

    // Close socket in signal handler.
    // Close file in signal handler.
    // Destroy pthread mutex in signal handler.

    return 0;
}

void *pthread_routine(void *param)
{
    pthread_arg_t *pthread_param;
    char secret_code[SECRET_CODE_LEN + 1];
    client_t client;

    char WELCOME_MESSAGE[GP_PAYLOAD_SIZE] = "\nWelcome to online Mastermind game.\n\n",
        INSTRUCTIONS_1_MESSAGE[GP_PAYLOAD_SIZE] =
        "Instructions: Win the game by correctly guessing the secret code within ten\n"
        "guesses. The code consists of four uppercase characters A, B, C, D, E and F.\n",
        INSTRUCTIONS_2_MESSAGE[GP_PAYLOAD_SIZE] =
        "Enter the guess when prompted with \">\". Do not include any spaces, lowercase\n"
        "letters or any other characters.\n\n",
        feedback_message[GP_PAYLOAD_SIZE];
    char buffer[GP_SIZE];
    int guesses_count = 0, correct_positions_count = 0, correct_colors_count;

    // Load parameters and free memory space allocated to hold param.
    pthread_param = (pthread_arg_t*)param;
    strncpy(secret_code, pthread_param->secret_code, SECRET_CODE_LEN + 1);
    client = pthread_param->client;
    free(param);

    // Log client connect.
    pthread_mutex_lock(&log_file_mutex);
    fprint_current_time_and_ip_address(log_file_fd, client.address);
    fprintf(log_file_fd, "(%d) client connected\n", client.new_socket_fd);
    fflush(log_file_fd);

    increment_clients_count();

    // Generate secret code if none provided.
    if (!is_valid_secret_code(secret_code))
    {
        get_random_secret_code(secret_code);
    }

    // Log server secret code.
    fprint_current_time_and_ip_address(log_file_fd, server_address);
    fprintf(log_file_fd, " server's secret = %s\n", secret_code);
    fflush(log_file_fd);
    pthread_mutex_unlock(&log_file_mutex);

    /******************** Begin communication with client. ********************/

    // Message welcome message to client.
    send_gp_message(&client, buffer, GP_DISP_MSG, WELCOME_MESSAGE);
    send_gp_message(&client, buffer, GP_DISP_MSG, INSTRUCTIONS_1_MESSAGE);
    send_gp_message(&client, buffer, GP_DISP_MSG, INSTRUCTIONS_2_MESSAGE);

    // While game has not ended.
    while (guesses_count < MAX_GUESSES_COUNT && correct_positions_count < SECRET_CODE_LEN)
    {
        // Request guess from client.
        send_gp_message(&client, buffer, GP_GUESS_REQ, NULL);

        /* If client enters invalid guess, repeat until client submits valid
         * guess.
         */
        while (1)
        {
            // Read guess sent from client.
            receive_message(client.new_socket_fd, buffer, GP_SIZE);
            // Guess is valid.
            if (is_valid_secret_code(buffer))
                break;
            // Guess is invalid, inform of invalid guess to client.
            else
                send_gp_message(&client, buffer, GP_GUESS_INV, NULL);
        }

        // Log client guess.
        pthread_mutex_lock(&log_file_mutex);
        fprint_current_time_and_ip_address(log_file_fd, client.address);
        fprintf(log_file_fd, "(%d) client's guess = %s\n", client.new_socket_fd, buffer);
        fflush(log_file_fd);

        // Get feedback from guess.
        get_secret_code_guess_feedback(secret_code, buffer, &correct_positions_count, &correct_colors_count);

        // Log server feedback.
        fprint_current_time_and_ip_address(log_file_fd, server_address);
        fprintf(log_file_fd, " server's hint = [%d:%d]\n", correct_positions_count, correct_colors_count);
        fflush(log_file_fd);
        pthread_mutex_unlock(&log_file_mutex);

        // Message feedback message to client.
        sprintf(
            feedback_message,
            "Guess: %d, Correct positions: %d, Correct colours: %d.\n",
            guesses_count + 1,
            correct_positions_count,
            correct_colors_count
            );
        send_gp_message(&client, buffer, GP_GUESS_FBK, feedback_message);

        guesses_count++;
    }

    // If client wins game, increment win count and deliver great news.
    if (correct_positions_count == SECRET_CODE_LEN)
    {
        increment_clients_win_count();

        // Log client win.
        pthread_mutex_lock(&log_file_mutex);
        fprint_current_time_and_ip_address(log_file_fd, client.address);
        fprintf(log_file_fd, "(%d) SUCCESS game over\n", client.new_socket_fd);
        fflush(log_file_fd);
        pthread_mutex_unlock(&log_file_mutex);

        send_gp_message(&client, buffer, GP_GAME_SUCC, NULL);
    }
    // If client lose, message client lose.
    else
    {
        // Log client lose.
        pthread_mutex_lock(&log_file_mutex);
        fprint_current_time_and_ip_address(log_file_fd, client.address);
        fprintf(log_file_fd, "(%d) FAILURE game over\n", client.new_socket_fd);
        fflush(log_file_fd);
        pthread_mutex_unlock(&log_file_mutex);

        send_gp_message(&client, buffer, GP_GAME_FAIL, NULL);
    }

    // Close socket.
    close(client.new_socket_fd);

    // Log client disconnect.
    pthread_mutex_lock(&log_file_mutex);
    fprint_current_time_and_ip_address(log_file_fd, client.address);
    fprintf(log_file_fd, "(%d) client disconnected\n", client.new_socket_fd);
    fflush(log_file_fd);
    pthread_mutex_unlock(&log_file_mutex);

    return NULL;
}
