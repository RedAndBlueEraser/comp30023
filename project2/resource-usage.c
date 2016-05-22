/*
 * resource-usage.c
 * Version 20160522
 * Written by Harry Wong (harryw1)
 * Adapted from http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource-usage.h"

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int parse_proc_self_status_line(char *line)
{
    int i = strlen(line);
    while (*line < '0' || *line > '9') line++;
    line[i - 3] = '\0';
    i = atoi(line);
    return i;
}

int get_proc_self_status_info(char *field)
{
    FILE* file = fopen("/proc/self/status", "r");
    int value = -1;
    char line[LINE_MAX_LEN];
    while (fgets(line, LINE_MAX_LEN, file) != NULL)
    {
        if (strncmp(line, field, strlen(field)) == 0)
        {
            value = parse_proc_self_status_line(line);
            break;
        }
    }
    fclose(file);
    return value;
}
