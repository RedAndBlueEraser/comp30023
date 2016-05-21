/*
 * secret-code.h
 * Version 20160518
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define SECRET_CODE_LEN        4
#define SECRET_CODE_COLOR_0    'A'
#define SECRET_CODE_COLOR_1    'B'
#define SECRET_CODE_COLOR_2    'C'
#define SECRET_CODE_COLOR_3    'D'
#define SECRET_CODE_COLOR_4    'E'
#define SECRET_CODE_COLOR_5    'F'
#define SECRET_CODE_COLORS_LEN 6

////////////////////////////////////////////////////////////////////////////////
// Function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* Get a random secret code and store in str. */
char *get_random_secret_code(char *secret_code);
/* Check if secret code is valid. */
int is_valid_secret_code(char *secret_code);
/* Get the feedback on the guess of the actual secret code. Returns 1 if correct
 * guess else 0.
 */
int get_secret_code_guess_feedback(
    char *actual,
    char *guess,
    int *correct_positions_count,
    int *correct_colors_count);
