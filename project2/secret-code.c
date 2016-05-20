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

/*
int main(int argc, char *argv[])
{
    int i;
    char sc[SECRET_CODE_LEN + 1];

    assert(is_valid_secret_code("AFGB") == 0);
    assert(is_valid_secret_code("ABZA") == 0);
    assert(is_valid_secret_code("ABBc") == 0);
    assert(is_valid_secret_code("ABB" ) == 0);
    assert(is_valid_secret_code("A BB") == 0);
    assert(is_valid_secret_code(" ABB") == 0);
    assert(is_valid_secret_code("ZZAA") == 0);
    assert(is_valid_secret_code("ABBBB") == 0);

    for (i = 0; i < 8; i++)
    {
        get_random_secret_code(sc);
        printf("%s\n", sc);
        assert(is_valid_secret_code(sc));
        sleep(1);
    }

    return 0;
}
*/
