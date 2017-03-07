#ifndef TYPESSS_H__
#define TYPESSS_H__

#include <limits.h>

#if ( __WORDSIZE == 64 )
#define U32 unsigned int
#define S32 short
#define U16 unsigned short
#define U8  unsigned char
#else //assume 32bit system
#define U32 unsigned long
#define S32 long
#define U16 unsigned short
#define U8  unsigned char
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#endif //TYPESSS_H__
