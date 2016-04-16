/*
 * simulation.c
 * Version 20160416
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "memory-management.h"
#include "process-data-file-parser.h"
#include "process-scheduler.h"

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define FCFS_ALGORITHM   0
#define MULTI_ALGORITHM  1

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
    char input, *filename;
    int algorithm, memsize;

    // Handle program arguments.
    while ((input = getopt(argc, argv, "f:a:m:")) != EOF)
    {
        switch (input)
        {
            case 'f':  // Filename of scheduled processes input.
                filename = optarg;
                break;
            case 'a':  // Algorithm to run.
                if (strcmp("fcfs", optarg) == 0)
                {
                    algorithm = FCFS_ALGORITHM;
                }
                else if (strcmp("multi", optarg) == 0)
                {
                    algorithm = MULTI_ALGORITHM;
                }
                else
                {
                    fprintf(stderr, "Invalid algorithm argument\n");
                    exit(1);
                }
                break;
            case 'm':  // Memory size.
                memsize = atoi(optarg);
                break;
            default:  // Unknown argument.
                fprintf(stderr, "Unknown argument\n");
                exit(1);
                break;
        }
    }

    // Run algorithm schedule.
    if (algorithm == FCFS_ALGORITHM)
    {
        fcfs_scheduler_runner(filename, memsize);
    }
    else if (algorithm == MULTI_ALGORITHM)
    {
        multi_scheduler_runner(filename, memsize);
    }

    return 0;
}
