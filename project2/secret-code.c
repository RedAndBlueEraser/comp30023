/*
 * secret-code.c
 * Version 20160518
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Libraries. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "secret-code.h"

////////////////////////////////////////////////////////////////////////////////
// Function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
char *get_random_secret_code(char *secret_code)
{
    const char SECRET_CODE_COLORS[] =
    {
        SECRET_CODE_COLOR_0, SECRET_CODE_COLOR_1, SECRET_CODE_COLOR_2,
        SECRET_CODE_COLOR_3, SECRET_CODE_COLOR_4, SECRET_CODE_COLOR_5
    };

    int i;

    // Fill with random colours.
    srand(time(NULL));
    for (i = 0; i < SECRET_CODE_LEN; i++)
    {
        secret_code[i] = SECRET_CODE_COLORS[rand() % SECRET_CODE_COLORS_LEN];
    }
    secret_code[SECRET_CODE_LEN] = '\0';

    return secret_code;
}

int is_valid_secret_code(char *secret_code)
{
    int i;

    // Iterate over array and check each colour.
    for (i = 0; i < SECRET_CODE_LEN; i++)
    {
        switch (secret_code[i])
        {
            case SECRET_CODE_COLOR_0:
            case SECRET_CODE_COLOR_1:
            case SECRET_CODE_COLOR_2:
            case SECRET_CODE_COLOR_3:
            case SECRET_CODE_COLOR_4:
            case SECRET_CODE_COLOR_5:
                continue;
                break;
            default:
                break;
        }
        break;
    }

    return i == SECRET_CODE_LEN && secret_code[SECRET_CODE_LEN] == '\0';
}

int get_secret_code_guess_feedback(
    char *actual,
    char *guess,
    int *correct_positions_count,
    int *correct_colors_count)
{
    int i, j, visited[SECRET_CODE_LEN] = { 0 }, visited_2[SECRET_CODE_LEN] = { 0 };

    *correct_positions_count = 0;
    *correct_colors_count = 0;

    // Count correct positions, marking which positions were visited.
    for (i = 0; i < SECRET_CODE_LEN; i++)
    {
        if (guess[i] == actual[i])
        {
            (*correct_positions_count)++;
            visited[i] = 1;
        }
    }

    /* Count correct colours, ignoring visited positions and marking which
     * positions of the actual secret code was visited.
     */
    for (i = 0; i < SECRET_CODE_LEN; i++)
    {
        if (visited[i])
        {
            continue;
        }
        else
        {
            for (j = 0; j < SECRET_CODE_LEN; j++)
            {
                if (visited[j] || visited_2[j])
                {
                    continue;
                }
                else if (guess[i] == actual[j])
                {
                    (*correct_colors_count)++;
                    visited_2[j] = 1;
                    break;
                }
            }
        }
    }

    return (*correct_positions_count) == SECRET_CODE_LEN;
}
