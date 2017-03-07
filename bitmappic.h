#ifndef BITMAPPIC_H__
#define BITMAPPIC_H__

#include "types.h"

///////////////////////////////////////////
///////////////////////////////////////////
typedef struct BMPFILEHEADER
{
    U16  FileType;     /* File type, always 4D42h ("BM") */
    U32  FileSize;     /* Size of the file in bytes */
    U16  Reserved1;    /* Always 0 */
    U16  Reserved2;    /* Always 0 */
    U32  BitmapOffset; /* Starting position of image data in bytes */
} BMPFILEHEADER;


typedef struct BITMAPHEADER
{
    U32 Size;            /* Size of this header in bytes */
    U32 Width;           /* Image width in pixels */
    U32 Height;          /* Image height in pixels */
    U16 Planes;          /* Number of color planes */
    U16 BitsPerPixel;    /* Number of bits per pixel */
    U32 Compression;     /* Compression methods used */
    U32 SizeOfBitmap;    /* Size of bitmap in bytes */
    U32 HorzResolution;  /* Horizontal resolution in pixels per meter */
    U32 VertResolution;  /* Vertical resolution in pixels per meter */
    U32 ColorsUsed;      /* Number of colors in the image */
    U32 ColorsImportant; /* Minimum number of important colors */
} BITMAPHEADER;

typedef struct BMP24BIT
{
  BMPFILEHEADER fileheader;
  BITMAPHEADER  bitmapheader;
  char *data_p;
} BMP24BIT;


typedef struct COLOR24BIT
{
  U8 blue;
  U8 green;
  U8 red;
} COLOR24BIT;

typedef struct PicInfo
{
  U32 width;
  U32 height;
  char *pbuf;
  char *pdata;
} PicInfo;


// Returns 0 only if all went well
U32 LoadPic(char * pszFile, struct PicInfo* pPicOut);


#endif // BITMAPPIC_H__
