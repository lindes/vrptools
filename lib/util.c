/* util.c - some random utility functions for VRPtools.
 *
 * Copyright 2013 by David Lindes.  All Rights reserved.
 *
 * Licensed according to LICENSE file, one directory up.
 */

#include <stdlib.h>

/* see https://en.wikipedia.org/wiki/English_numerals#Ordinal_numbers
 * Note, though, that I'm treating negatives as still ordinal (-1st for "negative first"). */
char *ordinal_suffix(int number)
{
    int ones = abs(number) % 10;
    int is_not_ten = (abs(number) % 100 / 10) != 1;

    return (ones == 1 && is_not_ten ? "st" : /* 1s, except 11s */
            ones == 2 && is_not_ten ? "nd" : /* 2s, except 12s */
            ones == 3 && is_not_ten ? "rd" : /* 3s, except 13s */
            "th"); /* everything else */
}
