/*
 * print_helpers.c -- helper functions for printing info from CINE files.
 *
 * part of vrptools -- https://github.com/lindes/vrptools
 *
 * Copyright 2011 by David Lindes.  All rights reserved.
 *
 * Available under terms in the LICENSE file that should accompany
 * this file.  Please consider that file to be included herein by
 * reference.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "vrptools.h"

void vrp_time_iso8601_s(VRP_TIME64 t, char *buf, int sz, int offset)
{
    struct tm *tm;
    time_t time;

    offset = -offset; /* they treat this opposite to tm_gmtoff */

    time = t.Seconds;
    /* compute break-out time once, just to get local offset: */
    tm = localtime(&time);
    /* adjust to the offset's timezone, removing our own: */
    time += offset - tm->tm_gmtoff;
    /* re-compute, still in our own timezone, but adjusted */
    tm = localtime(&time);

    buf += strftime(buf, sz, "%FT%T", tm);
    sprintf(buf, "%+03d%02d", offset / 3600, (abs(offset)) % 3600);
}

const char const *vrp_time_iso8601(VRP_TIME64 t, int offset)
{
    static char buf[BUFSIZ];
    vrp_time_iso8601_s(t, buf, BUFSIZ, offset);
    return buf;
}
