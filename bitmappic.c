
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "types.h"
#include "bitmappic.h"

// Returns 0 only if all went well
U32 LoadPic(char * pszFile, struct PicInfo* pPicOut)
{
  FILE *infile;
  size_t readsize;
  U32 filesize, bitmapoffset, dummy;
  char szFileType[2];
  char tmpbyte;
  char *ptmp, *ptmp1;
  int i, j;

  printf("Reading file %s ...\n", pszFile);
  assert(sizeof(U32) == 4);


  if(!pPicOut)
  {
    return 1;
  }

  infile = fopen(pszFile, "rb");
  if(!infile)
  {
    printf("Failed to open file %s for reading\n", pszFile);
    return 1;
  }

  readsize = fread(szFileType, 2, 1, infile);
  if(readsize != 1)
  {
    printf("Failed to read file type. File:%s\n", pszFile);
    fclose(infile);
    return 1;
  }

  // File type should be bitmap "BM"
  if((szFileType[0] != 'B') || (szFileType[1] != 'M'))
  {
    printf("Wrong file type. File: %s\n", pszFile);
    fclose(infile);
    return 1;
  }

  readsize = fread(&filesize, 4, 1, infile);
  if(readsize != 1)
  {
    printf("Failed to read file size. File: %s\n", pszFile);
    fclose(infile);
    return 1;
  }

  fread(&dummy, 4, 1, infile); // Dummy read of reserved words
  readsize = fread(&bitmapoffset, 4, 1, infile);
  if(readsize != 1)
  {
    printf("Failed to read bitmapoffset. File: %s\n", pszFile);
    fclose(infile);
    return 1;
  }

  fread(&dummy, 4, 1, infile); // Dummy read of header size
  readsize = fread(&(pPicOut->width), 4, 1, infile);
  if(readsize != 1)
  {
    printf("Failed to read width. File: %s\n", pszFile);
    fclose(infile);
    return 1;
  }

  readsize = fread(&(pPicOut->height), 4, 1, infile);
  if(readsize != 1)
  {
    printf("Failed to read height. File: %s\n", pszFile);
    fclose(infile);
    return 1;
  }


  pPicOut->pbuf = (char*)malloc(filesize);
  if(!pPicOut->pbuf)
  {
    printf("Out of memory. Allocation size: %d\n", filesize);
    fclose(infile);
    return 1;
  }

  if(fseek(infile, 0, SEEK_SET))
  {
    printf("Failed to seek beginning of file: %s\n", pszFile);
    fclose(infile);
    return 1;
  }

  // Read the whole file in one chunk
  readsize = fread(pPicOut->pbuf, filesize, 1, infile);
  if(readsize != 1)
  {
    printf("Failed to read file %s filesize=%d, readsize=%d\n", pszFile, filesize, (int)readsize);
    fclose(infile);
    return 1;
  }

  pPicOut->pdata = pPicOut->pbuf + bitmapoffset;

  printf("Read file %s\nsize: %d, bitmapoffset: %d\nwidth: %d\nheight: %d\n",
         pszFile, filesize, bitmapoffset, pPicOut->width, pPicOut->height);

  fclose(infile);


  // Convert BGR -> RGB
  ptmp = pPicOut->pdata;
  for (i = 0; i < (pPicOut->height * pPicOut->width); i++) {
      ptmp1 = ptmp;
      ptmp++;
      ptmp++;
      tmpbyte = *ptmp1;
      *ptmp1 = *ptmp;
      *ptmp = tmpbyte;
      ptmp++;
  }

  return 0;
}

