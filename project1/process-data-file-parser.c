/*
 * process-data-file-parser.h
 * Version 20160409
 * Written by Harry Wong
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process-data-file-parser.h"

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
scheduled_process_t *new_scheduled_process(int start_time, int process_id,
    int memory_size, int burst_time)
{
    scheduled_process_t *proc;

    // Allocate memory for scheduled process and error check.
    proc = (scheduled_process_t*)malloc(sizeof(scheduled_process_t));
    if (proc == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Set scheduled process.
    proc->start_time = start_time;
    proc->process_id = process_id;
    proc->memory_size = memory_size;
    proc->burst_time = burst_time;

    return proc;
}

scheduled_process_t **parse_process_data_file(FILE *fp)
{
    scheduled_process_t **scheduled_procs;
    int scheduled_procs_len = 0,
        scheduled_procs_max_len = INITIAL_SCHEDULED_PROCS_MAX_LEN;

    scheduled_process_t *proc;

    char line[LINE_MAX_LEN];

    char *tokens[TOKENS_LEN];
    int tokens_count;

    // Allocate array for scheduled processes and error check.
    scheduled_procs = (scheduled_process_t**)malloc(sizeof(scheduled_process_t*) * (scheduled_procs_max_len + 1));
    if (scheduled_procs == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Read in line from file.
    while (fgets(line, LINE_MAX_LEN, fp) != NULL)
    {
        // Get tokens.
        tokens_count = 0;
        tokens[tokens_count] = strtok(line, TOKEN_DELIMITER);
        tokens_count++;
        while (tokens[tokens_count - 1] != NULL && tokens_count < TOKENS_LEN)
        {
            tokens[tokens_count] = strtok(NULL, TOKEN_DELIMITER);
            tokens_count++;
        }

        // Verify enough tokens.
        if (tokens_count != TOKENS_LEN)
        {
            continue;
            /*fprintf(stderr, "Invalid file input\n");
            exit(1);*/
        }

        // Create new scheduled process.
        proc = new_scheduled_process(
            atoi(tokens[0]),  // Start time.
            atoi(tokens[1]),  // Process id.
            atoi(tokens[2]),  // Memory size.
            atoi(tokens[3])   // Burst time.
            );

        /* Add scheduled process to scheduled processes, realloc'ing first if
         * necessary.
         */
        if (scheduled_procs_len == scheduled_procs_max_len)
        {
            scheduled_procs_max_len *= SCHEDULED_PROCS_MAX_LEN_GROW_FACTOR;
            scheduled_procs = (scheduled_process_t**)realloc(scheduled_procs, sizeof(scheduled_process_t*) * (scheduled_procs_max_len + 1));
            if (scheduled_procs == NULL)
            {
                perror("realloc");
                exit(1);
            }
        }
        scheduled_procs[scheduled_procs_len] = proc;
        scheduled_procs_len++;
    }

    // Null-pointer terminate the array.
    scheduled_procs[scheduled_procs_len] = NULL;

    return scheduled_procs;
}

void print_scheduled_process(scheduled_process_t *sp)
{
    printf(
        "\"Start time: %d, Process id: %d, Memory size: %d, Burst time: %d\"",
        sp->start_time,
        sp->process_id,
        sp->memory_size,
        sp->burst_time
        );
    return;
}

void print_scheduled_processes(scheduled_process_t *sps[])
{
    scheduled_process_t *sp;
    printf("[");
    int i = 0;
    while ((sp = sps[i]) != NULL)
    {
        if (i != 0)
        {
            printf(",\n");
        }
        print_scheduled_process(sp);
        i++;
    }
    printf("]");
    return;
}

/* Test code. *//*
int main(int argc, char *argv[])
{
    FILE *fp;
    scheduled_process_t **sps;

    fp = fopen("specInput.txt", "r");
    sps = parse_process_data_file(fp);
    fclose(fp);

    print_scheduled_processes(sps);

    return 0;
}*/