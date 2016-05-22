/*
 * resource-usage.h
 * Version 20160522
 * Written by Harry Wong (harryw1)
 * Adapted from http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define LINE_MAX_LEN 128

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Parse a line in the proc self status file to extract an integer. Only works
 * with lines ending with " KB".
 */
int parse_proc_self_status_line(char *line);
/* Gets the value of a field in the proc self status file. Only works
 * with lines ending with " KB", so generally memory related fields.
 */
int get_proc_self_status_info(char *field);
