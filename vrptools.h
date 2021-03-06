/*
 * vrptools.h -- general header for vrptools
 *
 * part of vrptools -- https://github.com/lindes/vrptools
 *
 * Copyright 2011 by David Lindes.  All rights reserved.
 *
 * Available under terms in the LICENSE file that should accompany
 * this file.  Please consider that file to be included herein by
 * reference.
 */

#include <sys/types.h>
#include <sys/stat.h>

/** Base data types -- note: Intel little-endian **/
typedef char          VRP_BYTE;   /* BYTE in docs */
typedef u_char        VRP_CHAR;   /* CHAR in docs */
typedef u_int16_t     VRP_WORD;   /* WORD in docs */
typedef int16_t       VRP_SHORT;  /* SHORT or INT16 in docs */
typedef VRP_SHORT     VRP_INT16;  /* alias */
typedef int32_t       VRP_BOOL;   /* BOOL in docs;
                                   * Note: it's a 32-bit "logic value";
                                   * sign not specified.  Not using bool
                                   * from stdbool.h lest that not be
                                   * exactly 32 bits.
                                   */
typedef u_int32_t     VRP_DWORD;  /* DWORD or UINT in docs */
typedef VRP_DWORD     VRP_UINT;   /* alias */
typedef int32_t       VRP_LONG;   /* LONG or INT in docs */
typedef VRP_LONG      VRP_INT;    /* alias */
typedef float         VRP_FLOAT;  /* FLOAT in docs - 32 bit */
typedef double        VRP_DOUBLE; /* DOUBLE in docs - 64 bit */
typedef char*         VRP_STRING; /* "array of chars" */
typedef VRP_UINT      VRP_RECT[4]; /* NOT documented!  Guessing! (x,y,h,w?) */

/** Structures **/

/* TIME64 - stores time in fixed point, 32.32 bit structure */
typedef struct _VRP_TIME64 {
    VRP_DWORD      Fractions; /* "1/4Gig i.e. approx 1/4ns"; multiplied by 2**32 */
                              /* This field is first because the structure is a single little-endian 64-bit number. */
    VRP_DWORD      Seconds;   /* standard unix epoch; but unsigned, goes to 2106 */
} VRP_TIME64; /* TIME64 in docs */

/* IMFILTER - defines convolution filter to apply to an image */
typedef struct _VRP_IMFILTER {
    VRP_INT        Dim;       /* "Square kernel dimension 3,5" (?) */
    VRP_INT        Shifts;    /* Right-shifts of Coef (8 shifts = divide by 256) */
    VRP_INT        Bias;      /* "Bias to add at end" (?) */
    VRP_INT        Coef[5][5]; /* max allocation for 5x5 filter (?) */
} VRP_IMFILTER; /* IMFILTER in docs */

/* WBGAIN - white balance gains */
typedef struct _WBGAIN {
    VRP_FLOAT      R;         /* correction for red */
    VRP_FLOAT      B;         /* correction for blue */
} VRP_WBGAIN; /* WBGAIN in docs */

/* The following is described but not named in the docs: */
typedef VRP_LONG VRP_ImageNumber; /* signed; -/0/+ = before, at, and after trigger */

/** File format structures -- see also file_format.txt **/
typedef struct _VRP_CINEFILEHEADER {
    VRP_WORD       Type;        /* MUST be the letters CI */
    VRP_WORD       Headersize;  /* header size in bytes */
    VRP_WORD       Compression; /* compression type, as follows: */
#define VRP_CC_RGB   0 /* gray cines */
#define VRP_CC_JPEG  1 /* JPEG-compressed file */
#define VRP_CC_UNINT 2 /* uninterpolated color image -- see CFA field of SETUP */
    VRP_WORD       Version;     /* currently supported here: 1 */
    /* the "First" items below are relative to the trigger */
    VRP_LONG       FirstMovieImage; /* first image recorded by camera */
    VRP_DWORD      TotalImageCount; /* total frames recorded by camera */
    VRP_LONG       FirstImageNo;    /* first image saved to this file */
    VRP_DWORD      ImageCount;      /* count if images in file */
    /* offsets: */
    VRP_DWORD      OffImageHeader;  /* offset of BITMAPINFOHEADER */
    VRP_DWORD      OffSetup;        /* offset of SETUP */
    VRP_DWORD      OffImageOffsets; /* offset of array of image positions */
    VRP_TIME64     TriggerTime;     /* when the trigger fired */
} VRP_CINEFILEHEADER; /* CINEFILEHEADER in docs */

/* bitmap info -- same structure as windows bitmaps, but tweaked to
 * allow greater bit-depth */
typedef struct _VRP_BITMAPINFOHEADER {
    VRP_DWORD      biSize;          /* bytes required by structure (without palette) */
    VRP_LONG       biWidth;         /* width in pixels */
    VRP_LONG       biHeight;        /* height in pixels - bottom-up from lower-left for phantom */
    VRP_WORD       biPlanes;        /* planes - must be 1 */
    VRP_WORD       biBitCount;      /* bits per pixel - Phantom can have:
                                     * 8 or 16:  monochrome (different from windows;
                                     *           camera may only support 10,12,14)
                                     * 24 or 48: color RGB
                                     * color palette images not accepted
                                     */
    VRP_DWORD      biCompression;   /* only BI_RGB is supported */
#define BI_RGB 0                    /* per teh intarnets */
    VRP_DWORD      biSizeImage;     /* size of image in bytes */
    VRP_LONG       biXPelsPerMeter; /* horizontal resolution at sensor */
    VRP_LONG       biYPelsPerMeter; /* vertical resolution at sensor */
    VRP_DWORD      biClrUsed;       /* # colors in color table (used?) */
    VRP_DWORD      biClrImportant;  /* for 16,48bit, max sample value +1
                                     * (1024, 4096, or 16384 for 10,12,14 bit) */
} VRP_BITMAPINFOHEADER; /* BITMAPINFOHEADER in docs */

/* Sigh.  Unfortunately, there are a couple of fields here that try to
 * re-align themselves to either 2- or 4-byte boundaries, by default,
 * yet the structure setup is such that that's not where they occur.
 * To fix it, we use #pragma pack(1) -- first pushing a stack for this
 * pragma, which we pop again at the end of the SETUP structure.
 * See http://en.wikipedia.org/wiki/Data_structure_alignment (search
 * for pack, and/or pack(1)).
 */
#pragma pack(push)
#pragma pack(1)
typedef struct _VRP_SETUP {
    VRP_WORD       FrameRate16;     /* obsolete */
    VRP_WORD       Shutter16;       /* obsolete */
    VRP_WORD       PostTrigger16;   /* obsolete */
    VRP_WORD       FrameDelay16;    /* obsolete */
    VRP_WORD       AspectRatio;     /* obsolete */
    VRP_WORD       Contrast16;      /* obsolete */
    VRP_WORD       Bright16;        /* obsolete */
    VRP_BYTE       Rotate16;        /* obsolete -- note, despite the name, the docs say this is a BYTE. */
    VRP_BYTE       TimeAnnotation;  /* obsolete */
    VRP_BYTE       TrigCine;        /* obsolete */
    VRP_BYTE       TrigFrame;       /* Still used??? doesn't say not; says :
                                       0 = internal
                                       1 = external
                                       2 = locktoirig (?) */
    VRP_BYTE       ShutterOn;       /* obsolete */
#define VRP_MAXLENDESCRIPTION_OLD 121 /* value for this is not
                                       * actually documented;
                                       * fortunately, it was able to
                                       * be calculated from seeing
                                       * where the Mark field landed. */
    VRP_CHAR       DescriptionOld[VRP_MAXLENDESCRIPTION_OLD]; /* obsolete */
    VRP_WORD       Mark;            /* Marker for (new) setup structure */
    VRP_WORD       Length;          /* Length of the current version of SETUP */
    VRP_WORD       Binning;         /* Binning factor -- not used anymore */
    VRP_WORD       SigOption;       /* global signals options:
                                     * bit 0 = 0: records SamplesPerImage signal samples
                                     * bit 0 = 1: records maximum possible number of signal samples */
    VRP_SHORT      BinChannels;     /* # of binary channels read from Signal Acquisition Model (SAM) */
    VRP_BYTE       SamplesPerImage; /* # of samples acquired per image, binary & analog */
    VRP_CHAR       BinName[8][11];  /* 8 10-char (plus \0) binary signal names (first 8) */
    VRP_WORD       AnaOption;       /* global analog options: 1: single ended; 2: bipolar */
    VRP_SHORT      AnaChannels;     /* # of analog channels used */
    VRP_BYTE       Res6;            /* "to be ignored" == reserved == padding?? */
    VRP_BYTE       AnaBoard;        /* board type:
                                     * 0 = none
                                     * 1 = dsk (DSP system kit)
                                     * 2 = dsk+SAM
                                     * 3 = Data Translation DT9802
                                     * 4 = Data Translation DT3010 */
    VRP_SHORT      ChOption[8];     /* per-channel analog gain, 4-bit (values 1-8) (?) */
    VRP_FLOAT      AnaGain[8];      /* gain correction -- voltage to real measurement units */
    VRP_CHAR       AnaUnit[8][6];   /* measurement unit for ana channels - 5-char strings, \0-term */
    VRP_CHAR       AnaName[8][11];  /* channel name for first 8 analog channels (@10+\0) */
    VRP_LONG       lFirstImage;     /* "Range of images for continuous recording: first image" (?) */
    VRP_DWORD      dwImageCount;    /* image count for continuous and signal recording */
    VRP_SHORT      nQFactor;        /* Quality - for compressed files on continous; 2..255 */
    VRP_WORD       wCineFileType;   /* Cine file type - for continuous recording. */
    VRP_CHAR       szCinePath[4][65]; /* 4 paths to save cine files? I don't get it. */
    VRP_WORD       bMainsFreq;      /* true = 60Hz (USA); false = 50Hz (europe), for signal view in DSP */

    /* "Time board -- deprecated:" */
    VRP_BYTE       bTimeCode;
    VRP_BYTE       bPriority;
    VRP_WORD       wLeapSecDY;
    VRP_DOUBLE     dDelayTC;
    VRP_DOUBLE     dDelayPPS;
    VRP_WORD       GenBits;
    VRP_INT        Res1;
    VRP_INT        Res2;
    VRP_INT        Res3;

    /* dimensions for cameras since v4: */
    VRP_WORD       ImWidth;         /* Image width */
    VRP_WORD       ImHeight;        /* Image height */
    VRP_WORD       EDRShutter16;    /* replaced by EDRShutterNs */
    VRP_UINT       Serial;          /* S/N -- For firewire cameras, subtract 0x58000 for real S/N. */
    VRP_INT        Saturation;      /* Color saturation adjustment */
    VRP_BYTE       Res5;
    VRP_UINT       AutoExposure;    /* 0 = disable, 1 = lock at trigger, 3 = active after trigger */
    VRP_BOOL       bFlipH;          /* flip horizontally */
    VRP_BOOL       bFlipV;          /* flip vertically */
    VRP_UINT       Grid;            /* display in setup: 0 = none, 2 = cross hairs, 8 = grid with 8 intervals */
    VRP_UINT       FrameRate;       /* Frame Rate in fps */
    VRP_UINT       Shutter;         /* replaced by ShutterNs */
    VRP_UINT       EDRShutter;      /* replaced by EDRShutterNs */
    VRP_UINT       PostTrigger;     /* number of post-trigger frames */
    VRP_UINT       FrameDelay;      /* replaced by FrameDelayNs */
    VRP_BOOL       bEnableColor;    /* user option: 0 = force gray from color cameras */
    VRP_UINT       CameraVersion;   /* Version of camera hardware, without decimal point; e.g: (post Y2K)
                                     * Firewire:
                                     * 4, 5, 6
                                     * Ethernet:
                                     * 42, 43, 51, 7, 72, 73, 9, 91, 10, 650 (p65), 660 (hd) */
    VRP_UINT       FirmwareVersion; /* self-explanatory */
    VRP_UINT       SoftwareVersion; /* ditto */
    VRP_INT        RecordingTimeZone; /* as active during recording (offset in minutes from UTC??) */
    VRP_UINT       CFA;             /* Color Filter Array mode (code and bitmask, as follows) */

    /* code part in low byte: */
#define VRP_CFA_NONE              0 /* gray */
#define VRP_CFA_VRI               1 /* gbrg/rggb */
#define VRP_CFA_VRIV6             2 /* bggr/grbg */
#define VRP_CFA_BAYER             3 /* gb/rg */
#define VRP_CFA_BAYERFLIP         4 /* rg/gb */

    /* high byte has mask for color/gray heads on v6, v6.2 */
#define VRP_TLgray                0x80000000
#define VRP_TRgray                0x40000000
#define VRP_BLgray                0x20000000
#define VRP_BRgray                0x10000000 /* Brgray [sic] in docs */

    /* Image processing, -100..100, with 0 as neutral: */
    VRP_INT        Bright;          /* brightness */
    VRP_INT        Contrast;        /* contrast */
    VRP_INT        Gamma;           /* Gamma */
    VRP_UINT       Reserved1;

    VRP_UINT       AutoExpLevel;    /* auto exposure control level */
    VRP_UINT       AutoExpSpeed;    /* auto exposure control speed */
    VRP_RECT       AutoExpRect;     /* makes auto exposure only sensitive to this area */
    VRP_WBGAIN     WBGain[4];       /* Gain adjust on R, B components, for White Balance:
                                     * 1.0: do nothing
                                     * index 0: whole image at single head cameras (v4,5,7)
                                     *          and TopLeft head for v6, v6.2 (multihead)
                                     * index 1,2,3: TopRight, BottomLeft, BottomRight for multihead */
    VRP_INT        Rotate;          /* 0 = do nothing, +90 = CCW; -90 = CW */
    VRP_WBGAIN     WBView;          /* White Balance to apply on ViewCines */
    VRP_UINT       RealBPP;         /* Real number of bits per pixel:
                                     * 8 on v3,4,5,6,42,43,51,62,72,9
                                     * 8 or 12 on v7
                                     * 8,10,12,14 on 14-bit cameras
                                     * (Storage: 8 or 16 bits) */
    VRP_UINT       Conv8Min;        /* Min Value when converting to 8 bits with "first degree function" (?) */
    VRP_UINT       Conv8Max;        /* max value when converting */
    VRP_INT        FilterCode;      /* see PhInt docs */
    VRP_INT        FilterParam;     /* image processing optional param */
    VRP_IMFILTER   UF;              /* User Filter: 3x3 or 5x5 convolution */
    VRP_UINT       BlackCalSVer;    /* Software Version for Black Ref. */
    VRP_UINT       WhiteCalSVer;    /* ditto for White Calibration */
    VRP_UINT       GrayCalSVer;     /* ditto for Gray Calibration */
    VRP_BOOL       bStampTime;      /* Stamp time (in continuous recording):
                                     * 1 = absolute time,
                                     * 3 = from trigger (boolean, huh?  oy.) */
                                    /* Note: Based on their example data being 0, I'm guessing
                                     * they mean 0, not 3? */
    VRP_UINT       SoundDest;       /* sound device: 0=none, 1=speaker, 2=sound board */
    VRP_UINT       FRPSteps;        /* suplimentary steps in frame rate profile (0 = no profile) */
    VRP_INT        FRPImgNr[16];    /* Image Number for where rate and/or exposure changes
                                     * (up to 16 pts -- 4 in current cameras) */
    VRP_UINT       FRPRate[16];     /* New value for frame rate (fps) */
    VRP_UINT       FRPExp[16];      /* new exposure value, nanoseconds -- not implemented */
    VRP_INT        MCCnt;           /* Multicine (segmented) recording: partitions = cine count - 1.
                                     * preview cine does not need partition. */
    VRP_FLOAT      MCPercent[64];   /* % memory used for partitions; only 15 used currently */
    VRP_UINT       CICalib;         /* Calibration for current image -- "CSR" - current session ref.
                                     * bitmask of the following: */
#define VRP_CALIB_BLACKREF   1 /* BlackRef in docs */
#define VRP_CALIB_WHITECALIB 2 /* WhiteCalib in docs */
#define VRP_CALIB_GRAYCHECK  4 /* GrayCheck in docs */

    /* Calibration done with these 6 params: */
    VRP_UINT       CalibWidth;
    VRP_UINT       CalibHeight;
    VRP_UINT       CalibRate;       /* in fps */
    VRP_UINT       CalibExp;        /* exposure duration, microseconds */
    VRP_UINT       CalibEDR;        /* EDR, microseconds */
    VRP_UINT       CalibTemp;       /* Temperature [units?] */

    VRP_UINT       HeadSerial[4];   /* head serials for multi-head (Ethernet) cams (v6.2)
                                     * when multiple heads saved,
                                     * serials are non-zero; when one
                                     * head used, its serial is in
                                     * 0'th element, rest are
                                     * 0xffffffff */

    VRP_UINT       RangeCode;       /* describes range data format (?) */
    VRP_UINT       RangeSize;       /* Range data size per img */
    VRP_UINT       Decimation;      /* Factor to reduce framerate when sending to i3 external mem by fiber */
    VRP_UINT       MasterSerial;    /* Master camera's Serial for external sync.  0 means none (not a slave) */
    VRP_UINT       Sensor;          /* camera sensor code (?) */
    VRP_UINT       ShutterNs;       /* Exposure, nanoseconds */
    VRP_UINT       EDRShutterNs;    /* EDR exposure, nanoseconds */
    VRP_UINT       FrameDelayNs;    /* Frame delay, nanoseconds */

    /* when saving a side-stamped image, the information is outside
    the main image, whose position within this frame: */
    VRP_UINT       ImPosXAcq;
    VRP_UINT       ImPosYAcq;
    VRP_UINT       ImWidthAcq;
    VRP_UINT       ImHeightAcq;

    VRP_CHAR       Description[4096]; /* User description or comments */
} VRP_SETUP; /* SETUP in docs */
#pragma pack(pop)

/* Tagged blocks -- present if there's space between the end of SETUP and the beginning of ImageOffsets */

typedef struct _VRP_TAGGED_BLOCK {
    VRP_DWORD      BlockSize;      /* size of this tagged block, including header */
    VRP_WORD       Type;           /* types enumerated below */
    VRP_WORD       Reserved;       /* 1 in all blocks but last one, where it's
                                    * zero.  Which to me means it
                                    * ought to be called bLastBlock or
                                    * somesuch. */
    VRP_BYTE       Data[0];        /* really (BlockSize - 8) bytes. */
} VRP_TAGGED_BLOCK;

enum VRP_TAGGED_BLOCK_TYPE {
    VRP_TB_Analog_and_digital_signals = 1000, /* 0x3e8 -- no longer used */
    VRP_TB_Image_time                 = 1001, /* 0x3e9 -- no longer used -- TIME64 array and possible
                                               * DWORD array for time and exposure (respectively; the
                                               * latter only if BlockSize shows it's available) for each
                                               * *recorded* frame. */
    VRP_TB_Time_only                  = 1002, /* 0x3ea --- array of TIME64s for the *saved* images. */
    VRP_TB_Exposure_only              = 1003, /* 0x3eb -- array of DWORD representing exposure time in
                                               * each frame saved -- may vary due to auto-exposure.
                                               * Divide by 2**32 to get time in (fractional) seconds. */
    VRP_TB_Range_data                 = 1004, /* 0x03ec -- info about orientation and distance to subject
                                               * "not fully defined at this time and should be ignored" */
    VRP_TB_BinSig                     = 1005, /* 0x3ed -- binary signals recorded by SAM3 module.
                                               * multichannel and multisample per image -- stored 8
                                               * samples per byte; only for frames saved.  Number of
                                               * channels and samples-per-image stored in SETUP. */
    VRP_TB_AnaSig                     = 1006, /* 0x3ee -- analog signals from SAM3; 16 bits per sample.
                                               * channels and samples-per-image stored in SETUP */
};

typedef int64_t VRP_ImageOffset;

/* pImage[ImageCount] -- not sure if/what I need as far as structure for this */

/* sigh, the following needs to be two structures, because there's a variable-sized element
 * in the middle of the structure.  Breaking it up into AnnotationData and ImageData.  Both occur for
 * each image. */

typedef struct _VRP_ImageAnnotation {
    VRP_DWORD      AnnotationSize;    /* total length of annotation (including this size) */
    VRP_BYTE       Annotation[0];     /* actually AnnotationSize - 8 */
    /* use and structure of annotation not defined, except that the last DWORD should always
     * be the image size.  Annotation data may be absent, but the AnnotationSize will always be there */
    /* And yes, I know, the last two sentences contradict each other.  We'll see. Oh, wait... */
} VRP_ImageAnnotation;

/* offset of this is at ImageAnnotation + 4 - (AnnotationSize) */
typedef struct _VRP_ImageData {
    VRP_DWORD      ImageSize;         /* "Oh, wait" -- is this considered "the last two bytes..."? */
    /* example: 08 00 00 00  00 A6 0E 00 -- annotation size 8; image size 960,000 (800x600*2) */
    /* there are different ways this can be stored, I figure a union makes sense for this: */
    union _VRP_PixelData {
        VRP_BYTE   gray8;
        VRP_WORD   gray16;
        struct _VRP_PixelData_BGR8 {
            VRP_BYTE b;
            VRP_BYTE g;
            VRP_BYTE r;
        } bgr8;
        struct _VRP_PixelData_BGR16 {
            VRP_WORD b;
            VRP_WORD g;
            VRP_WORD r;
        } bgr16;
    } pixels[0];
} VRP_ImageData;

/* compressed files are proprietary.  Unless that changes, we won't be supporting them here. */

/* New structure for this project: */
typedef struct _VRP_File {
    char *name;
    int fd;
    struct stat st;

    VRP_CINEFILEHEADER   *header;
    VRP_BITMAPINFOHEADER *imageHeader;
    VRP_SETUP            *setup;
    VRP_TAGGED_BLOCK     *firstTaggedBlock;
    VRP_ImageOffset      *firstImageOffset;
    /* we probably don't really need this: */
    VRP_ImageAnnotation  *firstImageAnnotation;
    void                 *start, *end; /* convenience pointers */
} VRP_File;

typedef VRP_File *VRP_Handle;

VRP_Handle read_cine_fd(int fd, const char *name);
VRP_Handle read_cine(const char *filename);
void free_cine_file(VRP_Handle handle); /* doesn't free handle, just its contents */
void free_cine_handle(VRP_Handle handle); /* calls free_cine_file, then frees handle */
size_t vrp_image_size(VRP_Handle handle);
void vrp_time_iso8601_s(VRP_TIME64 t, char *buf, int sz, int offset);
const char const *vrp_time_iso8601(VRP_TIME64 t, int offset);
