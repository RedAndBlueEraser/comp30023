/*
 * memory-management.h
 * Version 20160410
 * Written by Harry Wong
 */

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
