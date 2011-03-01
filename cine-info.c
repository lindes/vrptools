/*
 * cineinfo.c -- simple command for displaying a bunch of info about a CINE file
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
#include <unistd.h> /* getopt() */

#include "vrptools.h"

void print_header_info(VRP_Handle handle)
{
    VRP_CINEFILEHEADER *h;

    if(!(h = handle->header))
    {
        fprintf(stderr, "-- No CINEFILEHEADER for %s\n", handle->name);
        return;
    }

    printf("CINEFILEHEADER info:\n");
    printf("  Name:              %s\n", handle->name);
    printf("  Type:              %2.2s (should be CI)\n", (const char *)&(h->Type));
    printf("  Headersize:        %d\n", h->Headersize);
    printf("  Compression:       %d (%s)\n", h->Compression,
           h->Compression == 0 ? "CC_RGB (gray)" :
           h->Compression == 1 ? "CC_JPEG" :
           h->Compression == 2 ? "CC_UNINT (uninterpolated)" : "[unknown]");
    printf("  Version:           %d\n", h->Version);
    printf("  Recorded images:   %d starting at %d\n", h->TotalImageCount, h->FirstMovieImage);
    printf("  Saved images:      %d starting at %d\n", h->ImageCount, h->FirstImageNo);
    printf("  Offsets:           %d (BITMAPINFOHEADER), %d (SETUP), %d (Image array)\n",
           h->OffImageHeader, h->OffSetup, h->OffImageOffsets);
    if(handle->setup)
        printf("  Trigger time:      %s\n", vrp_time_iso8601(h->TriggerTime, handle->setup->RecordingTimeZone));
}

void print_imageheader_info(VRP_Handle handle)
{
    VRP_BITMAPINFOHEADER *bmi;

    if(!(bmi = handle->imageHeader))
    {
        fprintf(stderr, "-- No BITMAPINFOHEADER in handle for %s\n", handle->name);
        return;
    }
    printf("BITMAPINFOHEADER info:\n");
    printf("  biSize:            %d\n", bmi->biSize);
    printf("  image size:        %dx%d (planes=%d) (bytes=%d)\n", bmi->biWidth, bmi->biHeight,
           bmi->biPlanes, bmi->biSizeImage);
    printf("  bits per pixel:    %d\n", bmi->biBitCount);
    printf("  biCompression:     %d\n", bmi->biCompression);
    printf("  sensor res.:       %dx%d (pixels per meter)\n", bmi->biXPelsPerMeter, bmi->biYPelsPerMeter);
    printf("  biClrUsed:         %d\n", bmi->biClrUsed);
    printf("  biClrImportant:    %d\n", bmi->biClrImportant);
}

void print_setup_info(VRP_Handle handle)
{
    VRP_SETUP *s;
    int i;

    if(!(s = handle->setup))
    {
        fprintf(stderr, "-- No SETUP in handle for %s\n", handle->name);
        return;
    }
    printf("SETUP info:\n");
/* #define PRINT_OLD_VALUES_TOO */
#ifdef PRINT_OLD_VALUES_TOO
    printf("  --NOTE: The following values are obsolete--\n");
    printf("    FrameRate16:       %d\n", s->FrameRate16);
    printf("    Shutter16:         %d\n", s->Shutter16);
    printf("    PostTrigger16:     %d\n", s->PostTrigger16);
    printf("    FrameDelay16:      %d\n", s->FrameDelay16);
    printf("    AspectRatio:       %d\n", s->AspectRatio);
    printf("    Contrast16:        %d\n", s->Contrast16);
    printf("    Bright16:          %d\n", s->Bright16);
    printf("    Rotate16:          %d\n", s->Rotate16);
    printf("    TimeAnnotation:    %d\n", s->TimeAnnotation);
    printf("    TrigCine:          %d\n", s->TrigCine);
    printf("    TrigFrame:         %d\n", s->TrigFrame);
    printf("    ShutterOn:         %d\n", s->ShutterOn);
    printf("    DescriptionOld:    %.*s\n", VRP_MAXLENDESCRIPTION_OLD, s->DescriptionOld);
    printf("  --END obsolete fields section--\n");
#endif
    printf("  Mark:              %2.2s/0x%x (should be ST)\n", (char*)(&s->Mark), s->Mark);
    printf("  Length:            %d (length of SETUP structure)\n", s->Length);
#ifdef PRINT_OLD_VALUES_TOO
    printf("  [obs.] Binning:    %d\n", s->Binning);
#endif
    printf("  SigOption:         %d\n", s->SigOption);
    printf("  BinChannels:       %d\n", s->BinChannels);
    printf("  SamplesPerImage:   %d\n", s->SamplesPerImage);
    printf("  BinName:           ");
    for(i = 0; i < 8; ++i)
        printf("'%.11s'%s", s->BinName[i], i < 7 ? ", " : "\n");
    printf("  AnaOption:         %d\n", s->AnaOption);
    printf("  AnaChannels:       %d\n", s->AnaChannels);
    printf("  AnaBoard:          %d\n", s->AnaBoard);
    printf("  ChOption:          ");
    for(i = 0; i < 8; ++i)
        printf("%d%s", s->ChOption[i], i < 7 ? ", " : "\n");
    printf("  AnaGain:           ");
    for(i = 0; i < 8; ++i)
        printf("%g%s", s->AnaGain[i], i < 7 ? ", " : "\n");
    printf("  AnaUnit:           ");
    for(i = 0; i < 8; ++i)
        printf("'%.6s'%s", s->AnaUnit[i], i < 7 ? ", " : "\n");
    printf("  AnaName:           ");
    for(i = 0; i < 8; ++i)
        printf("'%.11s'%s", s->AnaName[i], i < 7 ? ", " : "\n");
    printf("  lFirstImage:       %d\n", s->lFirstImage);
    printf("  dwImageCount:      %d\n", s->dwImageCount);
    printf("  nQFactor:          %d\n", s->nQFactor);
    printf("  wCineFileType:     %d\n", s->wCineFileType);
    printf("  szCinePath:      ");
    for(i = 0; i < 4; ++i)
        printf("'%.65s'%s", s->szCinePath[i], i < 3 ? ", " : "\n");
#if PRINT_OLD_VALUES_TOO
    printf("  bMainsFreq:        %d (%s)\n", s->bMainsFreq,
           s->bMainsFreq ? "60Hz (USA)" : "50Hz (europe)");
    printf("  -- Time board info; deprecated\n");
    printf("    bTimeCode:         %d\n", s->bTimeCode);
    printf("    bPriority:         %d\n", s->bPriority);
    printf("    wLeapSecDY:        %d\n", s->wLeapSecDY);
    printf("    dDelayTC:          %g\n", s->dDelayTC);
    printf("    dDelayPPS:         %g\n", s->dDelayPPS);
    printf("    GenBits:           %d\n", s->GenBits);
    printf("  -- end time board info\n");
#endif
    printf("  Dimensions:        %dx%d\n", s->ImWidth, s->ImHeight);
#ifdef PRINT_OLD_VALUES_TOO
    printf("  EDRShutter16:      %d\n", s->EDRShutter16);
#endif
    printf("  Serial #:          %1$d (0x%1$x)\n", s->Serial);
    if(s->Serial > 0x58000)
        printf("  Serial # (FW):   %1$d (0x%1$x)\n", s->Serial - 0x58000);
    printf("  Saturation:        %d\n", s->Saturation);
    printf("  AutoExposure:      %d\n", s->AutoExposure);
    printf("  flip? (h,v):       %d,%d\n", s->bFlipH, s->bFlipV);
    printf("  Grid:              %d\n", s->Grid);
    printf("  FrameRate:         %d fps\n", s->FrameRate);
    printf("  Shutter(old):      %d\n", s->Shutter);
    printf("  EDRShutter(old):   %d\n", s->EDRShutter);
    printf("  PostTrigger:       %d frames\n", s->PostTrigger);
    printf("  FrameDelay(old):   %d\n", s->FrameDelay);
    printf("  bEnableColor:      %d\n", s->bEnableColor);
    printf("  CameraVersion:     %d\n", s->CameraVersion);
    printf("  FirmwareVersion:   %d\n", s->FirmwareVersion);
    printf("  SoftwareVersion:   %d\n", s->SoftwareVersion);
    printf("  RecordingTimeZone: %d\n", s->RecordingTimeZone);
    printf("  CFA mode:          %d (%s)\n", s->CFA,
           s->CFA == 0 ? "CFA_NONE (gray)" :
           s->CFA == 1 ? "CFA_VRI (gbrg/rggb)" :
           s->CFA == 2 ? "CFA_VRIV6 (bggr/grbg)" :
           s->CFA == 3 ? "CFA_BAYER (gb/rg)" :
           s->CFA == 4 ? "CFA_BAYERFLIP (rg/gb)" : "[unknown]");
    // TODO: account for high bits (TLgray, TRgray, BLgray, BRgray)
    printf("  Bright:            %d\n", s->Bright);
    printf("  Contrast:          %d\n", s->Contrast);
    printf("  Gamma:             %d\n", s->Gamma);
    printf("  AutoExpLevel:      %d\n", s->AutoExpLevel);
    printf("  AutoExpSpeed:      %d\n", s->AutoExpSpeed);
    /* I'm not sure what this format is exactly, but mimicing the Appendix's output: */
    printf("  AutoExpRect:       %d,%d,%d,%d\n", s->AutoExpRect[0], s->AutoExpRect[2],
           s->AutoExpRect[1], s->AutoExpRect[3]);
    for(i = 0; i < 4; ++i)
        printf("  WBGain[%d]:         R: %.6f, B: %.6f\n", i, s->WBGain[i].R, s->WBGain[i].B);
    printf("  Rotate:            %d\n", s->Rotate);
    printf("  WBView:            R: %.6f, B: %.6f\n", s->WBView.R, s->WBView.B);
    printf("  RealBPP:           %d\n", s->RealBPP);
    printf("  Conv8Min:          %d\n", s->Conv8Min);
    printf("  Conv8Max:          %d\n", s->Conv8Max);
    printf("  FilterCode:        %d\n", s->FilterCode);
    printf("  FilterParam:       %d\n", s->FilterParam);
    printf("  UserFilter:        dim=%d, shifts=%d, bias=%d, coef =\n",
           s->UF.Dim, s->UF.Shifts, s->UF.Bias);
    for(i = 0; i < 5; ++i)
        printf("  %5d %5d %5d %5d %5d\n", s->UF.Coef[i][0], s->UF.Coef[i][1],
               s->UF.Coef[i][2], s->UF.Coef[i][3], s->UF.Coef[i][4]);
    printf("  BlackCalSVer:      %d\n", s->BlackCalSVer);
    printf("  WhiteCalSVer:      %d\n", s->WhiteCalSVer);
    printf("  GrayCalSVer:       %d\n", s->GrayCalSVer);
    printf("  bStampTime:        %d (%s)\n", s->bStampTime,
           s->bStampTime == 1 ? "absolute time" :
           s->bStampTime == 3 ? "from trigger" :
           s->bStampTime == 0 ? "from trigger?" : "[unknown]");
    printf("  SoundDest:         %d (%s)\n", s->SoundDest,
           s->SoundDest == 0 ? "none" :
           s->SoundDest == 1 ? "speaker" :
           s->SoundDest == 2 ? "sound board" : "[unknown]");
    printf("  FRPSteps:          %d\n", s->FRPSteps);
    printf("  FRPImgNr:          ");
    for(i = 0; i < 16; ++i)
        printf("%d%s", s->FRPImgNr[i], i < 15 ? ", " : "\n");
    printf("  FRPRate:           ");
    for(i = 0; i < 16; ++i)
        printf("%d%s", s->FRPRate[i], i < 15 ? ", " : "\n");
    printf("  FRPExp(not impl.): ");
    for(i = 0; i < 16; ++i)
        printf("%d%s", s->FRPExp[i], i < 15 ? ", " : "\n");
    printf("  MCCnt (multicine): %d\n", s->MCCnt);
    printf("  MCPercent:         \n");
    for(i = 0; i < 64; ++i)
        printf("%s%10.6f%s",
               i % 8 == 0 ? "    " : "",
               s->MCPercent[i],
               i % 8 == 7 ? (i < 63 ? ",\n" : "\n") : ", ");
    printf("  CICalib:           %d\n", s->CICalib);
    printf("  Calib size:        %dx%d\n", s->CalibWidth, s->CalibHeight);
    printf("  CalibRate:         %d\n", s->CalibRate);
    printf("  CalibExp:          %d%s\n", s->CalibExp, s->CalibExp ? " us" : "");
    printf("  CalibEDR:          %d%s\n", s->CalibEDR, s->CalibEDR ? " us" : "");
    printf("  CalibTemp:         %d\n", s->CalibTemp);
    printf("  HeadSerial:        ");
    for(i = 0; i < 4; ++i)
        printf("%d%s", s->HeadSerial[i], i < 3 ? ", " : "\n");
    printf("  RangeCode:         %d\n", s->RangeCode);
    printf("  RangeSize:         %d\n", s->RangeSize);
    printf("  Decimation:        %d\n", s->Decimation);
    printf("  MasterSerial:      %1$d (0x%1$x)\n", s->MasterSerial);
    printf("  Sensor:            %d\n", s->Sensor);
    printf("  ShutterNs:         %d\n", s->ShutterNs);
    printf("  EDRShutterNs:      %d\n", s->EDRShutterNs);
    printf("  FrameDelayNs:      %d\n", s->FrameDelayNs);
    printf("  ImPosXAcq:         %d\n", s->ImPosXAcq);
    printf("  ImPosYAcq:         %d\n", s->ImPosYAcq);
    printf("  ImWidthAcq:        %d\n", s->ImWidthAcq);
    printf("  ImHeightAcq:       %d\n", s->ImHeightAcq);
    printf("  Description:       '%.4096s'\n", s->Description);
}

void print_taggedblock_info(VRP_Handle handle)
{
    if(!handle->firstTaggedBlock)
    {
        fprintf(stderr, "-- No Tagged Block info for %s\n", handle->name);
        return;
    }
    printf("Tagged Block info reporting not completed.\n");
}

void print_image_info(VRP_Handle handle)
{
    if(!handle->firstImageAnnotation)
    {
        fprintf(stderr, "-- No firstImageInnotation in handle for %s\n", handle->name);
        return;
    }
    printf("Image Info reporting not completed.\n");
}

void print_summary(VRP_Handle handle)
{
    VRP_CINEFILEHEADER  *h = handle->header;
    VRP_SETUP           *s = handle->setup;
    char                *mode;
    int                 a, b, c, d, e, f;

    switch(h->Compression)
    {
    case 2 /* where's my constant? */: mode = "RAW"; break;
    default: mode = "[unknown]"; break;
    }

    printf(" %dx%d at %d fps (%s)\n", s->ImWidth, s->ImHeight, s->FrameRate, mode);

    a = h->FirstMovieImage;
    b = h->TotalImageCount;
    c = a + b;

    d = h->FirstImageNo;
    e = h->ImageCount;
    f = d + e;

    printf(" Recorded frames %d => %d (%d total); saved frames %d => %d (%d total)\n", a, c, b, d, f, e);
}

void usage(const char *name)
{
    fprintf(stderr, "usage: %s [-v]\n", name);
}

int main(int argc, char *argv[])
{
    int i;
    int verbose = 0;

    while((i = getopt(argc, argv, "v")) != -1)
    {
        switch(i)
        {
        case 'v': verbose = 1; break;
        default:
            usage(argv[0]);
            return -1;
        }
    }

    argc -= optind;
    argv += optind;


    for(i = 0; i < argc; ++i)
    {
        VRP_Handle handle;

        printf("--=> %s <=--\n", argv[i]);

        if(!(handle = read_cine(argv[i])))
        {
            fprintf(stderr, "Failed to get handle on %s\n", argv[i]);
            continue;
        }

        if(verbose)
        {
            print_header_info(handle);
            print_imageheader_info(handle);
            print_setup_info(handle);
            print_taggedblock_info(handle);
            print_image_info(handle);
        }
        else
        {
            print_summary(handle);
        }

        free_cine_handle(handle);
    }

    return(0);
}
