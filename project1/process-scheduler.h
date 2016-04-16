/*
 * process-scheduler.h
 * Version 20160416
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define MIN_PRIORITY     3
#define Q1_QUANTUM       2
#define Q2_QUANTUM       4
#define Q3_QUANTUM       8

////////////////////////////////////////////////////////////////////////////////
// Data structures. ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
enum process_state_t { ready, running, terminated };

/* Data structure to hold information of a process. */
typedef struct process_control_block_t
{
    struct process_control_block_t *next;
    int                            process_id;
    enum process_state_t           process_state;
    process_memory_t               *process_memory;
    int                            priority;    // Determines the queue its in.
    int                            job_time;    // Total time to finish process.
    int                            burst_time;  // Time ran continuously on CPU.
    int                            cpu_time;    // Total time ran on CPU.
} process_control_block_t;

typedef struct pcbs_queue_t
{
    struct process_control_block_t *head;
} pcbs_queue_t;

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Allocate memory for and initialise a new process control block. */
process_control_block_t *new_process_control_block(int process_id,
    int memory_size, int job_time, process_memories_list_t *proc_mems_list);
/* Allocate memory for and initialise a new process control block with a
 * scheduled process.
 */
process_control_block_t *new_process_control_block_by_scheduled_process(
    scheduled_process_t *sp, process_memories_list_t *proc_mems_list);
/* Allocate memory for and initialise a new process control block queue or list.
 */
pcbs_queue_t *new_pcbs_queue();
pcbs_queue_t *new_pcbs_list();
/* Add a process control block to the process control block queue, ordered by
 * priority then process number.
 */
void add_pcb_to_pcbs_queue(process_control_block_t *pcb, pcbs_queue_t *pcbs_queue);
/* Get and remove the first process control block in the process control block
 * queue or list.
 */
process_control_block_t *pop_front_pcb_from_pcbs_queue(pcbs_queue_t *pcbs_queue);
process_control_block_t *pop_front_pcb_from_pcbs_list(pcbs_queue_t *pcbs_list);
/* Append a process control block to the end of the process control block queue.
 */
void append_pcb_to_pcbs_list(process_control_block_t *pcb, pcbs_queue_t *pcbs_list);
/* Load the process control block's memory into main memory and update its
 * attributes to prepare to run.
 */
void load_pcb(process_control_block_t *pcb, int time,
    process_memories_list_t *proc_mems_list,
    free_memory_segments_list_t *mem_segs_list);
/* Run the process for one time step. */
void run_pcb(process_control_block_t *pcb);
/* Preempt the process and updates its attributes. This does NOT lower its
 * priority.
 */
void preempt_pcb(process_control_block_t *pcb);
/* Decrease priority and reset state to ready for the process. */
void ready_and_decrease_priority_pcb(process_control_block_t *pcb);
/* Termiante the process and release its memory from main memory. */
void terminate_pcb(process_control_block_t *pcb,
    free_memory_segments_list_t *mem_segs_list);
/* Check if the process has finished its job(-time). */
int is_pcb_finished(process_control_block_t *pcb);
/* Print the running process and memory statistics. */
void print_simulation_status(int time, process_control_block_t *running,
    process_memories_list_t *process_memories_list,
    free_memory_segments_list_t *free_list, int memsize);
/* Load, on arrival of, new processes in the ready queue using the first come
 * first serve algorithm.
 */
int check_scheduled_processes(scheduled_process_t *sps[], int spi, int time,
    pcbs_queue_t *ready_queue, process_memories_list_t *proc_mems_list);
/* Free all memory allocated for process control block queue or list. */
void free_pcbs_queue(pcbs_queue_t *pcbs_queue);
void free_pcbs_list(pcbs_queue_t *pcbs_list);
/* Get the quantum (time) of a process. */
int get_quantum_by_pcb(process_control_block_t *pcb);
/* Check if quantum is exhausted by process. */
int is_quantum_exhausted_by_pcb(process_control_block_t *pcb);
/* Run the first come first serve process scheduler. */
void fcfs_scheduler_runner(char filename[], int memsize);
/* Run the multi-level feedback queue process scheduler. */
void multi_scheduler_runner(char filename[], int memsize);
