/*
 * cine-extract.c -- extract one or more images from a CINE file.
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "vrptools.h"

void extract_to_ppm_dir(VRP_Handle handle, const char *outdir);

void extract_image_by_offset(VRP_Handle handle, int offset, 
			     int *rows_out, int *cols_out,
			     uint16_t **outbuf_out)
{
    VRP_ImageOffset     *theImagePointer;
    VRP_ImageAnnotation *theAnnotation;
    VRP_ImageData       *theImage;
    VRP_WORD            *pixelData;
    int                 i, j, row, col, rows, cols;
    int                 left, right, top, bottom;
    int                 bufsiz;
    uint16_t            *outbuf;
    float               wb_b, wb_r;
    struct _ppm_pixel {
        VRP_WORD r;
        VRP_WORD g;
        VRP_WORD b;
    } pixel;

    if(handle->header->Compression != VRP_CC_UNINT)
    {
        fprintf(stderr, "Woah, sorry, don't (yet) know how to handle Compression type %d\n",
                handle->header->Compression);
        return;
    }
    if(handle->setup->CFA != VRP_CFA_BAYER)
    {
        fprintf(stderr, "Woah, sorry, don't (yet) know how to handle CFA type %d\n", handle->setup->CFA);
        return;
    }

    theImagePointer = handle->firstImageOffset + offset;
    theAnnotation   = handle->start + *theImagePointer;
    theImage        = handle->start + *theImagePointer + theAnnotation->AnnotationSize - sizeof(VRP_DWORD);
    pixelData       = handle->start + *theImagePointer + theAnnotation->AnnotationSize;

    rows = handle->imageHeader->biHeight;
    cols = handle->imageHeader->biWidth;

    bufsiz = 3*rows*cols;

    wb_r = handle->setup->WBGain[0].R;
    wb_b = handle->setup->WBGain[0].B;

    outbuf = *outbuf_out;
    if(!outbuf)
    {
	outbuf=calloc(bufsiz, sizeof(*outbuf));
	if (!outbuf)
	{
	    perror("calloc");
	    return;
	}
	*outbuf_out = outbuf;
    }
    *rows_out = rows;
    *cols_out = cols;

    /* go through rows backwards, to convert bottom-up format to top-down: */
    for(i = rows - 1; i >= 0; --i)
    {
        row = rows - i - 1;

        /* columns go in regular (left-to-right) order: */
        for(j = 0; j < cols; ++j)
        {
            col = j;

            /* XXX very naive and simple demosaicing right now */
            /* TODO: find a better algorithm; see http://en.wikipedia.org/wiki/Demosaicing */

            left   = 2*(col/2);
            right  = left+1;
            top    = 2*(row/2);
            bottom = top + 1;

            switch((row % 2) << 1 | col % 2)
            {
            case 0: /* bottom-left: red */
                pixel.r = pixelData[row*cols+col]; /* real */
                pixel.g = pixelData[row*cols+col+1]; /* from the right */
                pixel.b = pixelData[(row+1)*cols+col+1]; /* from above, right */
                break;
            case 1: /* bottom-right: green */
                pixel.r = pixelData[row*cols+col-1]; /* from the left */
                pixel.g = pixelData[row*cols+col]; /* real value */
                pixel.b = pixelData[(row+1)*cols+col]; /* from above */
                break;
            case 2: /* top-left: green */
                pixel.r = pixelData[(row-1)*cols+col]; /* from below */
                pixel.g = pixelData[row*cols+col]; /* real value */
                pixel.b = pixelData[row*cols+col+1]; /* from the right */
                break;
            case 3: /* top-right: blue */
                pixel.r = pixelData[(row-1)*cols+col-1]; /* from below, left */
                pixel.g = pixelData[row*cols+col-1]; /* from the left */
                pixel.b = pixelData[row*cols+col]; /* real value */
                break;
            }

            /* adjust white balance */
            pixel.r = wb_r * pixel.r;
            if(pixel.r >= handle->imageHeader->biClrImportant)
                pixel.r = handle->imageHeader->biClrImportant - 1;
            pixel.b = wb_b * pixel.b;
            if(pixel.b >= handle->imageHeader->biClrImportant)
                pixel.b = handle->imageHeader->biClrImportant - 1;

            /* prepare to store -- using network byte order (big-endian) */
            pixel.r = htons(pixel.r);
            pixel.g = htons(pixel.g);
            pixel.b = htons(pixel.b);

            outbuf[3*(i*cols+j)+0] = pixel.r;
            outbuf[3*(i*cols+j)+1] = pixel.g;
            outbuf[3*(i*cols+j)+2] = pixel.b;
        }
    }
}


void extract_image_by_frame_id(VRP_Handle handle, int frame)
{
    int offset, last;

    offset = frame - handle->header->FirstImageNo;
    last = handle->header->FirstImageNo + handle->header->ImageCount;

    if(offset < 0 || (unsigned)offset > handle->header->ImageCount)
    {
        fprintf(stderr, "Asking for frame %d, which is outside of range %d -> %d\n",
                frame, handle->header->FirstImageNo, last);
    }
    fprintf(stderr, "frame %d means offset %d, fetching by offset\n", frame, offset);
}

int main(int argc, char *argv[])
{
    int i;
    const char *outdir = "cine-extract.d";

    for(i = 1; i < argc; ++i)
    {
        VRP_Handle handle;
        int first, trigger, last;
        
        if (!strcmp(argv[i], "-d")) {
            i ++;
            outdir = argv[i];
            if (!outdir) {
                fprintf(stderr, "Directory name must follow -d option\n");
                exit(1);
            }
            continue;
        }

        fprintf(stderr, "--=> reading %s <=--\n", argv[i]);

        if(!(handle = read_cine(argv[i])))
        {
            fprintf(stderr, "Failed to get handle on %s\n", argv[i]);
            continue;
        }

        if((first = handle->header->FirstImageNo) > 0)
        {
            fprintf(stderr, "Sorry, trigger frame is not saved in this file (starts with frame %d).\n", first);
        }

        /* the offset of the frame we actually want: */
        trigger = 0;

        last = handle->header->ImageCount + first;

        if(trigger < first)
        {
            fprintf(stderr, "NOTICE: trigger frame 0 not in range %d -> %d, setting to %d\n", first, last, first);
            trigger = first;
        }
        else if(trigger > last)
        {
            fprintf(stderr, "NOTICE: trigger frame 0 not in range %d -> %d, setting to %d\n", first, last, last);
            trigger = last;
        }

        fprintf(stderr, "DEBUG: first = %d, trig = %d, last = %d\n", first, trigger, last);

        fprintf(stderr, "Capturing the %d%s frame (frame #0 out of %d through %d)\n", trigger+1,
               trigger+1 % 10 == 1 && trigger+1 != 11 ? "st" :
               trigger+1 % 10 == 2 && trigger+1 != 12 ? "st" :
               trigger+1 % 10 == 3 && trigger+1 != 13 ? "st" :
               "th", first, last);


	extract_to_ppm_dir(handle, outdir);

        free_cine_handle(handle);
    }

    return(0);
}


void extract_to_ppm_dir(VRP_Handle handle, const char *outdir)
{
    unsigned int j;
    /* by default, all frames, 1 at a time */
    /* TODO: make these command-line args */
    int first = 0, increment = 1;
    uint16_t *outbuf = NULL;
    int rows, cols;

    if(first < 0) first = 0;
    
    for(j = first; j < handle->header->ImageCount; j += increment)
    {
	char filename[BUFSIZ];
	FILE *outfile;
	
	sprintf(filename, "%s/img-%05u.ppm", outdir, j);
	fprintf(stderr, "Extracting image at offset %d into %s\n", j, filename);
	
	if(!(outfile = fopen(filename, "wb")))
	{
	    perror(filename);
	    return;
	}
	
	extract_image_by_offset(handle, j, &rows, &cols, &outbuf);
	fprintf(outfile, "P6\n%d %d\n%d\n", cols, rows, handle->imageHeader->biClrImportant);
	fwrite(outbuf, sizeof(short), 3*cols*rows, outfile);
	
	fclose(outfile);
    }
    
    if (outbuf)
	free(outbuf);
}
