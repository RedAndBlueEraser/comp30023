/*
 * memory-management.h
 * Version 20160409
 * Written by Harry Wong
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
//#include "memory-management.h"

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Used in start_address when the process memory is in disk.
#define IN_DISK -1

////////////////////////////////////////////////////////////////////////////////
// Data structures. ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Data structure to hold information of a free memory segment. */
typedef struct free_memory_segment_t
{
    struct free_memory_segment_t *next;
    int start_address;
    int size;
} free_memory_segment_t;

/* Data structure to hold information of a list of free memory segments. */
typedef struct free_memory_segments_list_t
{
    free_memory_segment_t *head;
} free_memory_segments_list_t;

/* Data structure to hold information of a process memory. */
typedef struct process_memory_t
{
    struct process_memory_t *next;
    int start_address;
    int size;
    int swap_in_time;
    int process_id;
} process_memory_t;

/* Data structure to hold information of a list of process memories. */
typedef struct process_memories_list_t
{
    process_memory_t *head;
} process_memories_list_t;

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Allocate memory for and initialise a new free memory segments list. */
free_memory_segments_list_t *new_free_memory_segments_list(int size);
/* Allocate memory for and initialise a new free memory segment and add it, in
 * proper order, to the free memory segments list.
 */
void add_new_free_memory_segment_to_free_memory_segments_list(int start_address,
    int size, free_memory_segments_list_t *mem_segs_list);
/* Get the end address of a free memory segment. */
int get_free_memory_segment_end_address(free_memory_segment_t *mem_seg);
/* Consolidate contiguous free memory segments in the free memory segments
 * list.
 */
void consolidate_free_memory_segments_list(
    free_memory_segments_list_t *mem_segs_list);
/* Get the count of free memory segments in the free memory segments list. */
int get_free_memory_segments_list_count(
    free_memory_segments_list_t *mem_segs_list);
/* Allocate memory for and initialise a new process memories list. */
process_memories_list_t *new_process_memories_list_t();
/* Allocate memory for and initialise a new process memory. */
process_memory_t *new_process_memory(int size, int process_id);
/* Append a process memory to the end of the process memories list. */
void add_process_memory_to_process_memories_list(process_memory_t *new_proc_mem,
    process_memories_list_t *proc_mems_list);
/* Allocate memory for and initialise a new process memory and append it to the
 * end of the process memories list.
 */
void add_new_process_memory_to_process_memories_list(int size, int process_id,
    process_memories_list_t *proc_mems_list);
/* Get the end address of a process memory. */
int get_process_memory_end_address(process_memory_t *proc_mem);
/* Check if process memory is NOT in main memory. Used this confusing function
 * name because it would be even confusing for is_proc_mem_in_mem().
 */
int is_process_memory_in_disk(process_memory_t *proc_mem);
/* Remove a process memory from the process memories list and releases occupied
 * free memory segment.
 */
int remove_process_memory_by_process_id(int process_id,
    process_memories_list_t *proc_mems_list,
    free_memory_segments_list_t *mem_segs_list);
/* Get the process memory from the process memories list by process id. */
process_memory_t *get_process_memory_by_process_id(int process_id,
    process_memories_list_t *proc_mems_list);
/* Swaps in a process memory into a free memory segment, occupying it. */
void swap_in_process_memory(process_memory_t *proc_mem, int time,
    process_memories_list_t *proc_mems_list,
    free_memory_segments_list_t *mem_segs_list);
/* Swaps out the largest (and if equal largest, longest in memory) process
 * memory and releases occupied free memory segment.
 */
void swap_out_process_memory(process_memories_list_t *proc_mems_list,
    free_memory_segments_list_t *mem_segs_list);
/* Print information about the process memories list. */
void print_process_memories_list(process_memories_list_t *proc_mems_list);
/* Print information about the free memory segments list. */
void print_free_memory_segments_list(free_memory_segments_list_t *mem_segs_list);
/* Free all memory allocated for process memories list. */
void free_process_memories_list(process_memories_list_t *proc_mems_list);
/* Free all memory allocated for free memory segments list. */
void free_free_memory_segments_list(free_memory_segments_list_t *mem_segs_list);

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
free_memory_segments_list_t *new_free_memory_segments_list(int size)
{
    free_memory_segments_list_t *mem_segs_list;

    free_memory_segment_t *mem_seg;

    // Allocate memory for free memory segment and error check.
    mem_seg = (free_memory_segment_t*)malloc(sizeof(free_memory_segment_t));
    if (mem_seg == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Set free memory segment.
    mem_seg->next = NULL;
    mem_seg->start_address = size;
    mem_seg->size = size;

    // Allocate memory for free memory segments list and error check.
    mem_segs_list = (free_memory_segments_list_t*)malloc(sizeof(free_memory_segments_list_t));
    if (mem_segs_list == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Set free memory segments list.
    mem_segs_list->head = mem_seg;

    return mem_segs_list;
}

void add_new_free_memory_segment_to_free_memory_segments_list(int start_address,
    int size, free_memory_segments_list_t *mem_segs_list)
{
    free_memory_segment_t *mem_seg;

    free_memory_segment_t *prev_mem_seg, *curr_mem_seg;

    // Allocate memory for free memory segment and error check.
    mem_seg = (free_memory_segment_t*)malloc(sizeof(free_memory_segment_t));
    if (mem_seg == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Set free memory segment.
    mem_seg->next = NULL;
    mem_seg->start_address = start_address;
    mem_seg->size = size;

    // SPECIAL CASE: If list is empty, assign to head.
    if (mem_segs_list->head == NULL)
    {
        mem_segs_list->head = mem_seg;
    }
    else
    {
        // Iterate towards end of list.
        prev_mem_seg = NULL;
        curr_mem_seg = mem_segs_list->head;
        while (curr_mem_seg != NULL)
        {
            // Add free memory segment to sorted position.
            if (mem_seg->start_address >= curr_mem_seg->start_address)
            {
                // Connect and reassign list's head
                if (curr_mem_seg == mem_segs_list->head)
                {
                    mem_segs_list->head = mem_seg;
                    mem_seg->next = curr_mem_seg;
                }
                // Add free memory segment into list.
                else
                {
                    prev_mem_seg->next = mem_seg;
                    mem_seg->next = curr_mem_seg;
                }

                break;
            }

            prev_mem_seg = curr_mem_seg;
            curr_mem_seg = curr_mem_seg->next;
        }

        // SPECIAL CASE: Add to end of list.
        if (curr_mem_seg == NULL)
        {
            prev_mem_seg->next = mem_seg;
            mem_seg->next = curr_mem_seg;
        }
    }

    consolidate_free_memory_segments_list(mem_segs_list);
    return;
}

int get_free_memory_segment_end_address(free_memory_segment_t *mem_seg)
{
    return mem_seg->start_address - mem_seg->size;
}

void consolidate_free_memory_segments_list(
    free_memory_segments_list_t *mem_segs_list)
{
    free_memory_segment_t *curr_mem_seg = mem_segs_list->head, *next_mem_seg;
    int curr_mem_seg_end_add;

    // Iterate over list and consolidate contiguous free memory segments.
    while (curr_mem_seg != NULL)
    {
        // No more memory segments to consolidate.
        next_mem_seg = curr_mem_seg->next;
        if (next_mem_seg == NULL)
        {
            break;
        }

        // Memory segment ends where next memory segment starts, so consolidate.
        curr_mem_seg_end_add = get_free_memory_segment_end_address(curr_mem_seg);
        if (curr_mem_seg_end_add == next_mem_seg->start_address)
        {
            curr_mem_seg->next = next_mem_seg->next;
            curr_mem_seg->size += next_mem_seg->size;

            // Release memory.
            free(next_mem_seg);

            /* Don't go to next iteration, might be more contiguous memory to
             * consolidate.
             */
            next_mem_seg = curr_mem_seg->next;
        }
        else
        {
            curr_mem_seg = next_mem_seg;
            next_mem_seg = curr_mem_seg->next;
        }
    }

    return;
}

int get_free_memory_segments_list_count(
    free_memory_segments_list_t *mem_segs_list)
{
    int count = 0;

    free_memory_segment_t *mem_seg = mem_segs_list->head;

    // Iterate over list and count.
    while (mem_seg != NULL)
    {
        count++;

        mem_seg = mem_seg->next;
    }

    return count;
}

process_memories_list_t *new_process_memories_list_t()
{
    process_memories_list_t *proc_mems_list;

    // Allocate memory for process memories list and error check.
    proc_mems_list = (process_memories_list_t*)malloc(sizeof(process_memories_list_t));
    if (proc_mems_list == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Set process memories list.
    proc_mems_list->head = NULL;

    return proc_mems_list;
}

process_memory_t *new_process_memory(int size, int process_id)
{
    process_memory_t *proc_mem;

    // Allocate memory for process memory and error check.
    proc_mem = (process_memory_t*)malloc(sizeof(process_memory_t));
    if (proc_mem == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // Set process memory.
    proc_mem->next = NULL;
    proc_mem->start_address = IN_DISK;
    proc_mem->size = size;
    proc_mem->swap_in_time = 0;
    proc_mem->process_id = process_id;

    return proc_mem;
}

void add_process_memory_to_process_memories_list(process_memory_t *new_proc_mem,
    process_memories_list_t *proc_mems_list)
{
    process_memory_t *curr_proc_mem, *next_proc_mem;

    // If list is empty, add process as head/first element of list.
    if (proc_mems_list->head == NULL)
    {
        proc_mems_list->head = new_proc_mem;
    }
    // Append process to end of list.
    else
    {
        // Iterate to end of list.
        curr_proc_mem = proc_mems_list->head;
        next_proc_mem = curr_proc_mem->next;
        while (next_proc_mem != NULL)
        {
            curr_proc_mem = next_proc_mem;
            next_proc_mem = curr_proc_mem->next;
        }

        curr_proc_mem->next = new_proc_mem;
    }

    return;
}

void add_new_process_memory_to_process_memories_list(int size, int process_id,
    process_memories_list_t *proc_mems_list)
{
    process_memory_t *proc_mem = new_process_memory(size, process_id);
    add_process_memory_to_process_memories_list(proc_mem, proc_mems_list);
    return;
}

int get_process_memory_end_address(process_memory_t *proc_mem)
{
    return proc_mem->start_address - proc_mem->size;
}

int is_process_memory_in_disk(process_memory_t *proc_mem)
{
    return (proc_mem->start_address == IN_DISK);
}

int remove_process_memory_by_process_id(int process_id,
    process_memories_list_t *proc_mems_list,
    free_memory_segments_list_t *mem_segs_list)
{
    int result = 0;

    process_memory_t *prev_proc_mem, *curr_proc_mem;

    // List is empty, don't do anything.
    if (proc_mems_list->head == NULL)
    {
    }
    // Find process in list.
    else
    {
        // Iterate towards end of list.
        prev_proc_mem = NULL;
        curr_proc_mem = proc_mems_list->head;
        while (curr_proc_mem != NULL)
        {
            /* Remove process memory from list, add free memory segments (if
             * process memory is in RAM), and free memory.
             */
            if (curr_proc_mem->process_id == process_id)
            {
                result = 1;  // Found process memory matching process id.

                // Remove and reassign list's head.
                if (curr_proc_mem == proc_mems_list->head)
                {
                    proc_mems_list->head = curr_proc_mem->next;
                }
                // Remove process memory from list.
                else
                {
                    prev_proc_mem->next = curr_proc_mem->next;
                }

                /* Add free memory segment to list in sorted descending start
                 * address order.
                 */
                if (!is_process_memory_in_disk(curr_proc_mem))
                {
                    add_new_free_memory_segment_to_free_memory_segments_list(
                        curr_proc_mem->start_address,
                        curr_proc_mem->size,
                        mem_segs_list
                        );
                }

                // Free memory.
                free(curr_proc_mem);

                break;
            }

            prev_proc_mem = curr_proc_mem;
            curr_proc_mem = curr_proc_mem->next;
        }
    }

    return result;
}

process_memory_t *get_process_memory_by_process_id(int process_id,
    process_memories_list_t *proc_mems_list)
{
    process_memory_t *proc_mem;

    // Iterate over list and find process memory by process id.
    proc_mem = proc_mems_list->head;
    while (proc_mem != NULL)
    {
        if (proc_mem->process_id == process_id)
        {
            break;
        }

        proc_mem = proc_mem->next;
    }

    return proc_mem;
}

void swap_in_process_memory(process_memory_t *proc_mem, int time,
    process_memories_list_t *proc_mems_list,
    free_memory_segments_list_t *mem_segs_list)
{
    free_memory_segment_t *prev_mem_seg, *curr_mem_seg;

    while (is_process_memory_in_disk(proc_mem))
    {
        // Iterate over list and find memory segment with fittable size.
        prev_mem_seg = NULL;
        curr_mem_seg = mem_segs_list->head;
        while (curr_mem_seg != NULL)
        {
            // Find free memory segment with size greater than process.
            if (proc_mem->size <= curr_mem_seg->size)
            {
                // Occupy free memory segment.
                proc_mem->start_address = curr_mem_seg->start_address;
                proc_mem->swap_in_time = time;

                // Shrink free memory segment (unused by process).
                curr_mem_seg->start_address = get_process_memory_end_address(proc_mem);
                curr_mem_seg->size -= proc_mem->size;

                // Delete free memory segment, because memory segment size is 0.
                if (curr_mem_seg->size == 0)
                {
                    // Remove and reassign list's head.
                    if (curr_mem_seg == mem_segs_list->head)
                    {
                        mem_segs_list->head = curr_mem_seg->next;
                    }
                    // Remove memory segment from list.
                    else
                    {
                        prev_mem_seg->next = curr_mem_seg->next;
                    }

                    free(curr_mem_seg);
                }

                return;
            }

            prev_mem_seg = curr_mem_seg;
            curr_mem_seg = curr_mem_seg->next;
        }

        swap_out_process_memory(proc_mems_list, mem_segs_list);
    }

    return;
}

void swap_out_process_memory(process_memories_list_t *proc_mems_list,
    free_memory_segments_list_t *mem_segs_list)
{
    process_memory_t *swap_out_proc_mem, *proc_mem;

    /* Initialise swap_out_proc_mem with a process that can be swapped out to be
     * compared against.
     */
    proc_mem = proc_mems_list->head;
    while (proc_mem != NULL)
    {
        if (!is_process_memory_in_disk(proc_mem))
        {
            swap_out_proc_mem = proc_mem;
            break;
        }
        proc_mem = proc_mem->next;
    }

    // Iterate over list and find best process memory to swap out.
    while (proc_mem != NULL)
    {
        // Can only swap out processes in memory (not disk).
        if (!is_process_memory_in_disk(proc_mem))
        {
            // Find largest process memory.
            if (proc_mem->size > swap_out_proc_mem->size)
            {
                swap_out_proc_mem = proc_mem;
            }
            // If both largest, find process swapped in earlier.
            else if (proc_mem->size == swap_out_proc_mem->size &&
                proc_mem->swap_in_time <= swap_out_proc_mem->swap_in_time)
            {
                swap_out_proc_mem = proc_mem;
            }
        }
        proc_mem = proc_mem->next;
    }

    // Add memory occupied by process back as free memory segment into list2.
    add_new_free_memory_segment_to_free_memory_segments_list(
        swap_out_proc_mem->start_address,
        swap_out_proc_mem->size,
        mem_segs_list);

    // Select process to be swapped out. Update attributes.
    swap_out_proc_mem->start_address = IN_DISK;
    swap_out_proc_mem->swap_in_time = 0;

    return;
}

void print_process_memories_list(process_memories_list_t *proc_mems_list)
{
    process_memory_t *proc_mem;

    // Iterate over list and print information.
    proc_mem = proc_mems_list->head;
    printf("{\n");
    while (proc_mem != NULL)
    {
        printf("  [proc_mem: %p, next: %p, start_add: %d, size: %d, time: %d, pid: %d]\n",
            proc_mem,
            proc_mem->next,
            proc_mem->start_address,
            proc_mem->size,
            proc_mem->swap_in_time,
            proc_mem->process_id);
        proc_mem = proc_mem->next;
    }
    printf("}\n");

    return;
}

void print_free_memory_segments_list(free_memory_segments_list_t *mem_segs_list)
{
    free_memory_segment_t *mem_seg;

    // Iterate over list and print information.
    mem_seg = mem_segs_list->head;
    printf("{\n");
    while (mem_seg != NULL)
    {
        printf("  [mem_seg: %p, next: %p, start_add: %d, size: %d]\n",
            mem_seg,
            mem_seg->next,
            mem_seg->start_address,
            mem_seg->size);
        mem_seg = mem_seg->next;
    }
    printf("}\n");

    return;
}

void free_process_memories_list(process_memories_list_t *proc_mems_list)
{
    process_memory_t *curr_proc_mem, *next_proc_mem;

    // Iterate over list and free each process memory.
    curr_proc_mem = proc_mems_list->head;
    while (curr_proc_mem != NULL)
    {
        next_proc_mem = curr_proc_mem->next;
        free(curr_proc_mem);
        curr_proc_mem = next_proc_mem;
    }

    free(proc_mems_list);

    return;
}

void free_free_memory_segments_list(free_memory_segments_list_t *mem_segs_list)
{
    free_memory_segment_t *curr_mem_seg, *next_mem_seg;

    // Iterate over list and free each memory segment.
    curr_mem_seg = mem_segs_list->head;
    while (curr_mem_seg != NULL)
    {
        next_mem_seg = curr_mem_seg->next;
        free(curr_mem_seg);
        curr_mem_seg = next_mem_seg;
    }

    free(mem_segs_list);

    return;
}

/* Test code. */
int main(int argc, char *argv[])
{
    free_memory_segments_list_t *mem_segs_list;

    process_memories_list_t *proc_mems_list;

    process_memory_t *pm1, *pm2, *pm3, *pm4, *pm5, *pm6, *pm7, *pm8;

    while (1)
    {
        mem_segs_list = new_free_memory_segments_list(150);

        proc_mems_list = new_process_memories_list_t();

        pm1 = new_process_memory(20, 1);
        pm2 = new_process_memory(20, 2);
        pm3 = new_process_memory(20, 3);
        pm4 = new_process_memory(30, 4);
        pm5 = new_process_memory(30, 5);
        pm6 = new_process_memory(40, 6);
        pm7 = new_process_memory(60, 7);
        pm8 = new_process_memory(90, 8);

        add_process_memory_to_process_memories_list(pm1, proc_mems_list);
        add_process_memory_to_process_memories_list(pm2, proc_mems_list);
        add_process_memory_to_process_memories_list(pm3, proc_mems_list);
        add_process_memory_to_process_memories_list(pm4, proc_mems_list);

        swap_in_process_memory(pm1, 0, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm2, 0, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm3, 1, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm4, 1, proc_mems_list, mem_segs_list);

        add_process_memory_to_process_memories_list(pm5, proc_mems_list);
        swap_in_process_memory(pm5, 2, proc_mems_list, mem_segs_list);
        add_process_memory_to_process_memories_list(pm6, proc_mems_list);
        swap_in_process_memory(pm6, 2, proc_mems_list, mem_segs_list);
        add_process_memory_to_process_memories_list(pm7, proc_mems_list);
        swap_in_process_memory(pm7, 3, proc_mems_list, mem_segs_list);
        add_process_memory_to_process_memories_list(pm8, proc_mems_list);
        swap_in_process_memory(pm8, 3, proc_mems_list, mem_segs_list);

        add_new_process_memory_to_process_memories_list(10, 9, proc_mems_list);
        add_new_process_memory_to_process_memories_list(20, 10, proc_mems_list);
        add_new_process_memory_to_process_memories_list(30, 11, proc_mems_list);
        add_new_process_memory_to_process_memories_list(40, 12, proc_mems_list);

        swap_in_process_memory(pm1, 4, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm2, 6, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm3, 7, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm4, 9, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm5, 10, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm6, 12, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm7, 13, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm8, 15, proc_mems_list, mem_segs_list);
        swap_in_process_memory(get_process_memory_by_process_id(9, proc_mems_list), 16, proc_mems_list, mem_segs_list);
        swap_in_process_memory(get_process_memory_by_process_id(10, proc_mems_list), 17, proc_mems_list, mem_segs_list);
        swap_in_process_memory(get_process_memory_by_process_id(11, proc_mems_list), 18, proc_mems_list, mem_segs_list);
        swap_in_process_memory(get_process_memory_by_process_id(12, proc_mems_list), 19, proc_mems_list, mem_segs_list);

        swap_in_process_memory(pm1, 4, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm2, 6, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm3, 7, proc_mems_list, mem_segs_list);
        swap_in_process_memory(pm4, 9, proc_mems_list, mem_segs_list);

        printf("\n======================================================\n");
        print_process_memories_list(proc_mems_list);
        printf("------------------------------------------------------\n");
        print_free_memory_segments_list(mem_segs_list);
        printf("\n======================================================\n");

        remove_process_memory_by_process_id(1, proc_mems_list, mem_segs_list);
        remove_process_memory_by_process_id(2, proc_mems_list, mem_segs_list);
        remove_process_memory_by_process_id(3, proc_mems_list, mem_segs_list);
        remove_process_memory_by_process_id(4, proc_mems_list, mem_segs_list);
        remove_process_memory_by_process_id(5, proc_mems_list, mem_segs_list);
        remove_process_memory_by_process_id(6, proc_mems_list, mem_segs_list);
        remove_process_memory_by_process_id(7, proc_mems_list, mem_segs_list);
        remove_process_memory_by_process_id(8, proc_mems_list, mem_segs_list);

        printf("\n======================================================\n");
        print_process_memories_list(proc_mems_list);
        printf("------------------------------------------------------\n");
        print_free_memory_segments_list(mem_segs_list);
        printf("\n======================================================\n");

        free(proc_mems_list);
        free(mem_segs_list);
        //sleep(1);
        break;
    }

    return 0;
}
