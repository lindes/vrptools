/*
 * read_cine.c -- API for reading a CINE file from Vision Research's Phantom cameras
 *
 * part of vrptools -- https://github.com/lindes/vrptools
 *
 * Copyright 2011 by David Lindes.  All rights reserved.
 *
 * Available under terms in the LICENSE file that should accompany
 * this file.  Please consider that file to be included herein by
 * reference.
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
    size_t      expected_size;

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

    if((size_t)handle->st.st_size < sizeof(VRP_CINEFILEHEADER))
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

    handle->start = handle->header; /* convenience pointer */

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

    /* note: doing this as an addition on the left of < rather than
     * subtraction to the right is important -- dealing with unsigned
     * values. */
    if(handle->header->OffImageHeader + sizeof(VRP_BITMAPINFOHEADER) < (size_t)handle->st.st_size)
        handle->imageHeader = handle->start + handle->header->OffImageHeader;
    else
        fprintf(stderr, "WARNING: %s is too small to contain Image Headers!\n", handle->name);

    if(handle->header->OffSetup + sizeof(VRP_SETUP) < (size_t)handle->st.st_size)
        handle->setup = handle->start + handle->header->OffSetup;
    else
        fprintf(stderr, "WARNING: %s is too small to contain Setup info!\n", handle->name);

    if(handle->header->OffImageOffsets > handle->header->OffSetup + sizeof(VRP_SETUP))
    {
        if(handle->header->OffSetup + sizeof(VRP_SETUP) + sizeof(VRP_TAGGED_BLOCK) < (size_t)handle->st.st_size)
            handle->firstTaggedBlock = (void*)handle->setup + sizeof(VRP_SETUP);
        else
            fprintf(stderr, "WARNING: It seems we ought to have tagged blocks, but we don't actually have space for them!\n");
    }
    else
        fprintf(stderr, "INFO: No tagged blocks found.\n");

    expected_size = handle->header->OffImageOffsets + handle->header->ImageCount * vrp_image_size(handle);

    if((size_t)handle->st.st_size < expected_size)
    {
        fprintf(stderr, "WARNING: file appears to be truncated (not all images are present)"
                " (size %llu, expected at least %lu.)\n", handle->st.st_size, expected_size);
    }

    handle->end = handle->start + handle->st.st_size;

    /* set it anyway, so we can at least get some images, if we have
     * them.  This could cause the client to have problems, but we
     * should be able to mostly solve it by just providing an API for
     * getting the address of a particular image. */
    handle->firstImageOffset     = handle->start + handle->header->OffImageOffsets;
    if((void*)handle->firstImageOffset > handle->end)
        handle->firstImageOffset = NULL;
    else
        handle->firstImageAnnotation = handle->start + *(handle->firstImageOffset);


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
        if(munmap(handle->header, handle->st.st_size))
            perror("munmap failed");

    /* only name has to be freed, everything else was under the mmap. */
    if(handle->name) free(handle->name);

    /* but even mmap-based data should be reset: */
    handle->header               = NULL;
    handle->imageHeader          = NULL;
    handle->setup                = NULL;
    handle->firstTaggedBlock     = NULL;
    handle->firstImageAnnotation = NULL;
}

void free_cine_handle(VRP_Handle handle)
{
    if(!handle) return;

    free_cine_file(handle);
    free(handle);
}

/* report the size, in bytes, of any one image file, as calculated by
 * appropriate header flags (including bit depth, width, height, etc.)
 * Note: Does *not* include the annotation. */
size_t vrp_image_size(VRP_Handle handle)
{
    if(!handle->imageHeader)
        return -1;

    return(handle->imageHeader->biSizeImage);
}
