/*
 * read_cine.c -- API for reading a CINE file from Vision Research's Phantom cameras
 *
 * Copyright 2011 by David Lindes.  All rights reserved.
 */

#include <stdio.h> /* for perror() */
#include <fcntl.h> /* for open() */
#include <string.h> /* for strlen() */
#include <sys/stat.h> /* for fstat(), struct stat */
#include <sys/mman.h> /* for mmap() */
#include <stdlib.h> /* for calloc() */

#include "vrptools.h"

VRP_Handle read_cine_fd(int fd, const char *name)
{
    VRP_Handle  handle;

    if(!(handle = calloc(sizeof(VRP_File), 1)))
    {
        perror("calloc");
        return NULL;
    }

    if(fstat(fd, &handle->st) < 0)
    {
        perror(name);
        return NULL;
    }

    if(handle->st.st_size < sizeof(VRP_CINEFILEHEADER))
    {
        fprintf(stderr, "%s: too small to be a CINE file!\n", name);
        free_cine_handle(handle);
        return NULL;
    }

    if(!(handle->header = mmap(NULL, handle->st.st_size, PROT_READ, MAP_SHARED, fd, 0)))
    {
        perror("mmap");
        free(handle);
        return NULL;
    }

    /* after this point, if we bail, we want to do it in a consistent way: */
#define BAIL free_cine_handle(handle); return NULL

    /* a couple very basic sanity checks before we do anything else: */

    /* check magic number */
    if(htons(handle->header->Type) != ('C' << 8 | 'I')
       || handle->header->Headersize != 44)
    {
        fprintf(stderr, "%s: does not appear to be a CINE file!\n", name);
        BAIL;
    }
    if(handle->header->Version != 1)
    {
        fprintf(stderr, "%s: version number (%d) not supported\n",
                name, handle->header->Version);
        /* a little extra error if the Version seems extraordinary. 5 is arbitrary. */
        if(handle->header->Version > 5)
            fprintf(stderr, "(Is this really a CINE file?)\n");

        BAIL;
    }

    handle->fd = fd;
    handle->name = strdup(name);

    return handle;
}

VRP_Handle read_cine(const char *filename)
{
    int fd;

    if(!filename) return NULL; /* TODO: error message? */
    if(strlen(filename) == 0) return NULL; /* TODO: error message? */

    /* treat "-" as stdin.  TODO: check to see if ./- exists first? */
    if(strlen(filename) == 1 && filename[0] == '-')
        fd = 0;
    else
        fd = open(filename, O_RDONLY);

    if(fd < 0)
    {
        perror(filename);
        return NULL;
    }

    return read_cine_fd(fd, filename);
}

void free_cine_file(VRP_Handle handle)
{
    if(!handle) return;

    if(handle->header)
    {
        struct stat st;

        munmap(handle->header, handle->st.st_size);
    }

    if(handle->name) free(handle->name);

    handle->header = NULL;

    /* TODO: add other fields */
}

void free_cine_handle(VRP_Handle handle)
{
    if(!handle) return;

    free_cine_file(handle);
    free(handle);
}
