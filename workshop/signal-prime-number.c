/*
 * signal-prime-number.c
 * Version 20160316
 * Written by Harry Wong
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
// function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Handle signals.
 * @param signo signal number
 */
void sighandler(int signo);

/**
 * Check if number is a prime number.
 * @param n number to be checked
 * @return 1 if number is a prime number else 0
 */
int isprimeno(int n);

////////////////////////////////////////////////////////////////////////////////
// function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void sighandler(int signo)
{
    switch (signo)
    {
        // receiving signal that parent process found a prime number
        case SIGUSR1:
            printf("A prime number was found by the parent process\n");
            break;
        // receiving signal that parent process is ending
        case SIGUSR2:
            printf("Parent process is ending, I must go now. :(\n");
            exit(0);
            break;
        default:
            break;
    }

    return;
}

int isprimeno(int n)
{
    int i;

    // number is less than 2
    if (n < 2)
    {
        return 0;
    }

    // iterate through all possible factors of number
    for (i = n / 2; i >= 2; i--)
    {
        // found a factor that divides the number completely
        if (n % i == 0)
        {
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    int n;
    pid_t pid;

    // set signal handler function and error check
    if (signal(SIGUSR1, sighandler) == SIG_ERR)
    {
        printf("Signal error\n");
        return 1;
    }
    if (signal(SIGUSR2, sighandler) == SIG_ERR)
    {
        printf("Signal error\n");
        return 1;
    }

    // fork a child process and failure check
    if ((pid = fork()) < 0)
    {
        printf("Fork failed\n");
        return 1;
    }

    // running as child process
    if (pid == 0)
    {
        while (1)
        {
            sleep(1);
        }
    }
    // running as parent process
    else
    {
        // ask user to input number
        printf("Enter numbers: (enter non-integer values to quit)\n");
        while (scanf("%d", &n))
        {
            // number is a prime number
            if (isprimeno(n))
            {
                // send signal to inform child process of discovery
                if (kill(pid, SIGUSR1) == -1)
                {
                    printf("Kill failed\n");
                    return 1;
                }
            }
        }

        // send signal to inform child process of termination
        if (kill(pid, SIGUSR2) == -1)
        {
            printf("Kill failed\n");
            return 1;
        }
    }

    return 0;
}