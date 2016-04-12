/*
 * process-scheduler.c
 * Version 20160412
 * Written by Harry Wong
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "memory-management.h"
#include "process-data-file-parser.h"
#include "process-scheduler.h"

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
process_control_block_t *new_process_control_block(int process_id,
    int memory_size, int job_time, process_memories_list_t *proc_mems_list)
{
    process_control_block_t *pcb;

    // Allocate memory for pcb and error check.
    pcb = (process_control_block_t*)malloc(sizeof(process_control_block_t));
    if (pcb == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Set pcb.
    pcb->next = NULL;
    pcb->process_id = process_id;
    pcb->process_state = ready;
    pcb->process_memory = new_process_memory(memory_size, process_id);
    add_process_memory_to_process_memories_list(pcb->process_memory, proc_mems_list);
    pcb->priority = 1;
    pcb->job_time = job_time;
    pcb->burst_time = 0;
    pcb->cpu_time = 0;

    return pcb;
}

process_control_block_t *new_process_control_block_by_scheduled_process(
    scheduled_process_t *sp, process_memories_list_t *proc_mems_list)
{
    return new_process_control_block(
        sp->process_id,
        sp->memory_size,
        sp->job_time,
        proc_mems_list
        );
}

pcbs_queue_t *new_pcbs_queue()
{
    pcbs_queue_t *pcbs_queue;

    // Allocate memory for pcb queue and error check.
    pcbs_queue = (pcbs_queue_t*)malloc(sizeof(pcbs_queue_t));
    if (pcbs_queue == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Set pcb queue.
    pcbs_queue->head = NULL;

    return pcbs_queue;
}

pcbs_queue_t *new_pcbs_list()
{
    return new_pcbs_queue();
}

void add_pcb_to_pcbs_queue(process_control_block_t *pcb, pcbs_queue_t *pcbs_queue)
{
    process_control_block_t *prev_pcb, *curr_pcb;

    // If queue is empty, add pcb as head.
    if (pcbs_queue->head == NULL)
    {
        pcbs_queue->head = pcb;
    }
    // Add process to queue according to priority then pid.
    else
    {
        // Iterate to end of queue.
        prev_pcb = NULL;
        curr_pcb = pcbs_queue->head;
        while (curr_pcb != NULL)
        {
            // Add process to sorted position.
            /* If pcb's priority is higher (smaller value) than curr_pcb's
             * priority, then insert before curr_pcb.
             *
             * OR if pcb's priority is equal to curr_pcb's priority AND if pcb's
             * process id is higher (smaller value) than curr_pcb's process id,
             * then insert before curr_pcb.
             */
            if (pcb->priority < curr_pcb->priority ||
                (pcb->priority == curr_pcb->priority && pcb->process_id < curr_pcb->process_id))
            {
                // Connect and reassign queue's head.
                if (curr_pcb == pcbs_queue->head)
                {
                    pcbs_queue->head = pcb;
                    pcb->next = curr_pcb;
                }
                // Insert into queue.
                else
                {
                    prev_pcb->next = pcb;
                    pcb->next = curr_pcb;
                }

                break;
            }

            prev_pcb = curr_pcb;
            curr_pcb = curr_pcb->next;
        }
        /* Append to end of queue if we iterated through the entire list and
         * found no pcbs that goes lower than the one we want to insert.
         */
        if (curr_pcb == NULL)
        {
            prev_pcb->next = pcb;
            pcb->next = curr_pcb;
        }
    }

    return;
}

process_control_block_t *pop_front_pcb_from_pcbs_queue(pcbs_queue_t *pcbs_queue)
{
    process_control_block_t *pcb = pcbs_queue->head;
    if (pcb != NULL)
    {
        pcbs_queue->head = pcb->next;
    }
    return pcb;
}

void append_pcb_to_pcbs_list(process_control_block_t *pcb, pcbs_queue_t *pcbs_list)
{
    process_control_block_t *prev_pcb, *curr_pcb;

    // If list is empty, add pcb as head.
    if (pcbs_list->head == NULL)
    {
        pcbs_list->head = pcb;
    }
    // Add process to list.
    else
    {
        // Iterate to end of list.
        prev_pcb = NULL;
        curr_pcb = pcbs_list->head;
        while (curr_pcb != NULL)
        {
            prev_pcb = curr_pcb;
            curr_pcb = curr_pcb->next;
        }
        // Append to end of list.
        prev_pcb->next = pcb;
        pcb->next = curr_pcb;
    }
    return;
}

process_control_block_t *pop_front_pcb_from_pcbs_list(pcbs_queue_t *pcbs_list)
{
    return pop_front_pcb_from_pcbs_queue(pcbs_list);
}

void load_pcb(process_control_block_t *pcb, int time,
    process_memories_list_t *proc_mems_list,
    free_memory_segments_list_t *mem_segs_list)
{
    // If process is not in main memory, swap it into main memory.
    if (is_process_memory_in_disk(pcb->process_memory))
    {
        swap_in_process_memory(
            pcb->process_memory, time, proc_mems_list, mem_segs_list
            );
    }

    // Set pcb.
    pcb->next = NULL;
    pcb->process_state = running;
    pcb->burst_time = 0;

    return;
}

void run_pcb(process_control_block_t *pcb)
{
    // Set pcb - increment burst time and cpu time.
    pcb->burst_time++;
    pcb->cpu_time++;
    return;
}

void preempt_pcb(process_control_block_t *pcb)
{
    // Set pcb.
    pcb->process_state = ready;
    return;
}

/* TODO: void preempt_pcb(process_control_block_t *pcb)
{
    // Set pcb - lower priority.
    pcb->priority = (pcb->priority + 1 > MIN_PRIORITY) ? MIN_PRIORITY : pcb->priority + 1;
    pcb->process_state = ready;
    pcb->burst_time = 0;
    return;
}*/

void terminate_pcb(process_control_block_t *pcb,
    free_memory_segments_list_t *mem_segs_list)
{
    // Swap out mememory.
    if (!is_process_memory_in_disk(pcb->process_memory))
    {
        swap_out_process_memory_by_process_memory(
            pcb->process_memory, mem_segs_list
            );
    }

    // Set pcb.
    pcb->next = NULL;
    pcb->process_state = terminated;

    return;
}

int is_pcb_finished(process_control_block_t *pcb)
{
    return (pcb->cpu_time >= pcb->job_time);
}

void print_simulation_status(int time, process_control_block_t *running,
    process_memories_list_t *process_memories_list,
    free_memory_segments_list_t *free_list, int memsize)
{
    // Calculate memory usage as a percentage.
    int memusagesize = get_process_memories_list_not_in_disk_size(process_memories_list);
    int memusageproportion = 100 * memusagesize;
    memusageproportion = memusageproportion / memsize + (memusageproportion % memsize != 0);

    printf("time %d, %d running, numprocesses=%d, numholes=%d, memusage=%d%%\n",
        time,
        running->process_id,
        get_process_memories_list_not_in_disk_count(process_memories_list),
        get_free_memory_segments_list_count(free_list),
        memusageproportion
        );
    return;
}

int fcfs__check_scheduled_processes(scheduled_process_t **sps, int spi,
    int time, pcbs_queue_t *ready_queue, process_memories_list_t *proc_mems_list)
{
    scheduled_process_t *next_scheduled_process = sps[spi];

    /* If next scheduled process exists and it is time to "start/load" process,
     * add it to queue.
     */
    if (next_scheduled_process != NULL &&
        next_scheduled_process->start_time <= time)
    {
        /* Create a new pcb and add it to pcbs queue and add its process memory
         * to process memories list.
         */
        append_pcb_to_pcbs_list(
            new_process_control_block_by_scheduled_process(
                next_scheduled_process,
                proc_mems_list
                ),
            ready_queue);
        spi++;
    }

    return spi;
}

void free_pcbs_queue(pcbs_queue_t *pcbs_queue)
{
    process_control_block_t *curr_pcb, *next_pcb;

    // Iterate over queue and free each pcb.
    curr_pcb = (pcbs_queue != NULL) ? NULL : pcbs_queue->head;
    while (curr_pcb != NULL)
    {
        next_pcb = curr_pcb->next;
        free(curr_pcb);
        curr_pcb = next_pcb;
    }

    free(pcbs_queue);

    return;
}

void free_pcbs_list(pcbs_queue_t *pcbs_list)
{
    free_pcbs_queue(pcbs_list);
    return;
}

// Multi level shit.
// TODO: int multi__check_scheduled_processes(scheduled_process_t **sps, int spi,
//           int time, pcbs_queue_t *ready_queue, process_memories_list_t *proc_mems_list);
// TODO: vod multi__scheduler
// TODO: int get_quantum_by_process_control_block(process_control_block_t *pcb);
// TODO: int is_quantum_finished_by_process_control_block(process_control_block_t *pcb);

void fcfs_scheduler_runner(char filename[], int memsize)
{
    int time = 0;  // Time steps.

    // Currently executing process.
    process_control_block_t *running = NULL;
    // Processes that need to be executed.
    pcbs_queue_t *ready_queue = new_pcbs_queue(),
    // Processes that finished executing.
        *terminated_list = new_pcbs_list();

    // Pointer to file holding scheduled processes.
    FILE *fp;
    // Scheduled processes.
    scheduled_process_t **scheduled_processes;
    // Index of scheduled processes.
    int spi = 0;

    // Load scheduled processes from file.
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }
    scheduled_processes = parse_process_data_file(fp);
    fclose(fp);
    fp = NULL;

    // Process memory images list.
    process_memories_list_t *process_memories_list = new_process_memories_list();
    // Free memory segments list.
    free_memory_segments_list_t *free_list = new_free_memory_segments_list(memsize);

    while (1)
    {
        // Create and add new processes from scheduled processes if need be.
        spi = fcfs__check_scheduled_processes(
            scheduled_processes, spi, time, ready_queue, process_memories_list
            );

        // No currently executing process, load next process in queue.
        if (running == NULL)
        {
            running = pop_front_pcb_from_pcbs_queue(ready_queue);

            // Load next process.
            if (running != NULL)
            {
                load_pcb(running, time, process_memories_list, free_list);
                print_simulation_status(time, running, process_memories_list, free_list, memsize);
            }
            /* Still no process running. Check if there are more incoming. If
             * break out of loop and exit.
             */
            else if (ready_queue->head == NULL && scheduled_processes[spi] == NULL)
            {
                break;
            }
        }

        // Execute process for one time step.
        if (running != NULL)
        {
            run_pcb(running);
        }

        // Next time step.
        //sleep(1);
        time++;

        /* Current executing process finished, terminate it and make it no
         * currently executing process.
         */
        if (running != NULL && is_pcb_finished(running))
        {
            terminate_pcb(running, free_list);
            append_pcb_to_pcbs_list(running, terminated_list);
            running = NULL;
        }
    }

    // Print end simulation message.
    printf("time %d, simulation finished.\n", time);

    // Free allocated memory.
    free_pcbs_queue(ready_queue);
    free_pcbs_list(terminated_list);
    free_scheduled_processes(scheduled_processes);
    free_free_memory_segments_list(free_list);
    free_process_memories_list(process_memories_list);

    return;
}

// TODO: LARGE: multi-level fb cue

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
        // TODO: multi_scheduler_runner(filename, memsize);
    }

    return 0;
}
