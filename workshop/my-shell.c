/*
 * my-shell.c
 * Version 20160316
 * Written by Harry Wong
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define CMD_PROMPT "> "
#define CMD_EXIT_COMMAND "exit"
#define STRSCAN_INITIAL_MAX_STR_LEN 64
#define STRSCAN_STR_LEN_GROW_FACTOR 2
#define STRSPLIT_INITIAL_MAX_STR_ARRAY_LEN 1
#define STRSPLIT_STR_ARRAY_LEN_GROW_FACTOR 2

////////////////////////////////////////////////////////////////////////////////
// global variables. ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
pid_t childpid;  // pid of child process

////////////////////////////////////////////////////////////////////////////////
// function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Handle signals.
 * @param signo signal number
 */
void sighandler(int signo);

/**
 * Compares the two strings if they are equivalent.
 * @param s1 first string
 * @param s2 second string
 * @return 0 if strings are equal
 */
int strcmp(char *s1, char *s2);

/**
 * Returns the length of the null-terminated string.
 * @param str null-terminated string
 * @return length of string excluding null byte
 */
size_t strlen(char *str);

/**
 * Prints the null-terminated string to stdout.
 * @param str null-terminated string
 */
void strprint(char *str);

/**
 * Returns a null-terminated string from stdin, prompting user for input. Stops
 * reading from stdin as soon as a newline character is read. The newline
 * character is changed to a null byte.
 * @return null-terminated string from stdin
 */
char *strscan();

/**
 * Returns an array of strings that resulted from splitting a string at the
 * delimitter character. The array ends with a NULL pointer.
 * @param origstr   null-terminated string
 * @param delimiter delimiter character
 * @return array of strings ending with a NULL pointer
 */
char **strsplit(char *origstr, char delimiter);

/**
 * Release all the memory allocated to store an array of memory allocated
 * strings.
 * @param strarray array of strings ending with a NULL pointer
 */
void freestrarray(char **strarray);

////////////////////////////////////////////////////////////////////////////////
// function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void sighandler(int signo)
{
    // receiving signal that timeout occurred
    if (signo == SIGALRM)
    {
        // kill child process
        strprint("Process timed out\n");
        if (kill(childpid, SIGKILL) == -1)
        {
            perror("kill");
            exit(1);
        }
    }
    return;
}

int strcmp(char *s1, char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

size_t strlen(char *str)
{
    int len = 0;

    // count each character in string until null byte
    while (str[len] != '\0')
    {
        len++;
    }

    return len;
}

void strprint(char *str)
{
    // write output to stdout
    if (write(STDOUT_FILENO, str, strlen(str)) < 0)
    {
        perror("write");
        exit(1);
    }

    return;
}

char *strscan()
{
    char *s;
    int slen = 0,
        maxslen = STRSCAN_INITIAL_MAX_STR_LEN;

    // allocate memory to store string
    if ((s = (char*)malloc(sizeof(char) * (maxslen + 1))) == NULL)
    {
        perror("malloc");
        exit(1);
    }

    while (1)
    {
        // expand memory to store string
        if (slen >= maxslen)
        {
            maxslen *= STRSCAN_STR_LEN_GROW_FACTOR;
            // reallocate memory to store string
            if ((s = realloc(s, sizeof(char) * (maxslen + 1))) == NULL)
            {
                perror("realloc");
                exit(1);
            }
        }

        // read input from stdin, one character at a time
        if (read(STDIN_FILENO, s + slen, 1) < 0)
        {
            perror("read");
            exit(1);
        }
        slen++;

        // last character read was newline - change to null byte and stop loop
        if (s[slen - 1] == '\n')
        {
            s[slen - 1] = '\0';
            break;
        }
    }

    return s;
}

char **strsplit(char *origstr, char delimiter)
{
    int origstrlen = strlen(origstr);

    char **strarray;
    int strarraylen = 0,
        maxstrarraylen = STRSPLIT_INITIAL_MAX_STR_ARRAY_LEN;

    int strarrayi,  // index of start of string token
        strarrayj;  // index of end of string token

    char *strtoken;
    int strtokenlen, strtokeni;

    // allocate memory to store string array
    if ((strarray = (char**)malloc(sizeof(char*) * (maxstrarraylen + 1))) == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // iterate through characters in the original string
    strarrayi = 0;
    while (strarrayi < origstrlen)
    {
        // character is a delimiter character, reloop at next character
        if (origstr[strarrayi] == delimiter)
        {
            strarrayi++;
            continue;
        }

        /* index of end of string token should be at least at index of start of
         * string token
         */
        strarrayj = strarrayi;

        // iterate through characters in the original string
        while (strarrayj < origstrlen)
        {
            /* character is a delimiter character, found index of end of string
             * token
             */
            if (origstr[strarrayj] == delimiter)
            {
                break;
            }
            strarrayj++;
        }

        strtokenlen = strarrayj - strarrayi;  // get string token length
        // allocate memory to store string token
        if ((strtoken = (char*)malloc(sizeof(char) * (strtokenlen + 1))) == NULL)
        {
            perror("malloc");
            exit(1);
        }
        // copy characters into string token
        for (strtokeni = 0; strtokeni < strtokenlen; strtokeni++)
        {
            strtoken[strtokeni] = origstr[strarrayi + strtokeni];
        }
        strtoken[strtokeni] = '\0';  // terminate string token with null byte

        strarrayi = strarrayj;  /* index of start of next string token should be
                                 * at least at index of end of this string token
                                 */

        // expand memory to store string token in string array
        if (strarraylen >= maxstrarraylen)
        {
            maxstrarraylen *= STRSPLIT_STR_ARRAY_LEN_GROW_FACTOR;
            if ((strarray = (char**)realloc(strarray, sizeof(char*) * (maxstrarraylen + 1))) == NULL)
            {
                perror("realloc");
                exit(1);
            }
        }

        // store string token in string array
        strarray[strarraylen] = strtoken;
        strarraylen++;
    }

    // array ends with null pointer
    strarray[strarraylen] = NULL;

    return strarray;
}

void freestrarray(char **strarray)
{
    char **strpointer = strarray;

    // free memory allocated for each string in array
    while (*strpointer != NULL)
    {
        free(*strpointer);
        strpointer++;
    }

    // free memory allocated for array
    free(strarray);

    return;
}
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    char *cmd, **cmdargv, *cmdfilename, **cmdenvp = { NULL };
    int timeout = (argc > 1) ? atoi(argv[1]) : 0;
    pid_t pid;

    // timeout exists
    if (timeout > 0)
    {
        // set signal handler function and error check
        if (signal(SIGALRM, sighandler) == SIG_ERR)
        {
            perror("signal");
            return 1;
        }
    }

    while (1)
    {
        // command line prompt
        strprint(CMD_PROMPT);

        // get input from user
        cmd = strscan();

        // get cmd's filename and argv
        cmdargv = strsplit(cmd, ' ');
        cmdfilename = cmdargv[0];
        // not cmdfilename, don't do anything
        if (cmdfilename == NULL)
        {
            continue;
        }
        // cmdfilename is exit, so end loop and exit shell
        else if (strcmp(cmdfilename, "exit") == 0)
        {
            break;
        }

        // fork a child process and failure check
        if ((pid = fork()) < 0)
        {
            perror("fork");
            return 1;
        }

        // running as child process
        if (pid == 0)
        {
            // execute program and failure check
            if (execve(cmdfilename, cmdargv, cmdenvp) < 0)
            {
                perror("execve");
                return 1;
            }
            return 1;
        }
        // running as parent process
        else
        {
            // set global variable childpid
            childpid = pid;
            // alarm timeout (kill child process via childpid)
            alarm(timeout);

            // wait for child process to finish
            wait(NULL);

            alarm(0);  // stop alarm timeout; child process finished on time

            // free memory taken up by cmd string
            free(cmd);
            freestrarray(cmdargv);
        }
    }

    return 0;
}
