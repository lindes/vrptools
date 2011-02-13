/*
 * vrptools.h -- general header for vrptools
 */

#include <sys/types.h>

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

/** Structures **/

/* TIME64 - stores time in fixed point, 32.32 bit structure */
typedef struct _VRP_TIME64 {
    VRP_DWORD      Fractions; /* "1/4Gig i.e. approx 1/4ns"; multiplied by 2**32 */
                              /* listed first in the docs... seems weird, but ok. */
    VRP_DWORD      Seconds;   /* standard unix epoch; but unsigned, goes to 2106 */
} VRP_TIME64; /* TIME64 in docs */

/* IMFILTER - defines convolution filter to apply to an image */
typedef struct _VRP_IMFILTER {
    VRP_INT        Dim;       /* "Square kernel dimension 3,5" (?) */
    VRP_INT        Shifts;    /* Right-shifts of Coef (8 shifts = divide by 256) */
    VRP_INT        Bias;      /* "Bias to add at end" (?) */
    VRP_INT        Coef[5*5]; /* max allocation for 5x5 filter (?) */
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
#define VRP_CC_RGB   0; /* gray cines */
#define VRP_CC_JPEG  1; /* JPEG-compressed file */
#define VRP_CC_UNINT 2; /* uninterpolated color image -- see CFA field of SETUP */
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
