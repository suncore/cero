
#ifndef _BASE_H
#define _BASE_H

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifdef  NULL
#undef  NULL
#endif
#define NULL    0

/* SIGN(-xxx) = -1, SIGN(0) = 0, SIGN(xxx) = 1 */
#ifndef SIGN
#define SIGN(n)   (0 < (n) ? 1 : ((n) < 0 ? -1 : 0))
#endif
#ifndef ABS
#define ABS(n)    (0 < (n) ? (n) : -(n))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((b) < (a) ? (a) : (b))
#endif

// If x is NaN (Floating point Not A Number then we return zero with this macro
#define DENAN(x)    	(x != x ? 0 : x)

/* Floating point CLAMP(): make sure the variable x is within the bounds of low...high */
#define FCLAMP(low, x, high) ((x) < (low) ? (low) : ((high) < (x) ? (high) : (x)))
/* Integer CLAMP(): make sure the variable x is within the bounds of low...high-1 */
#define ICLAMP(low, x, high) ((x) < (low) ? (low) : ((high) <= (x) ? (high-1) : (x)))

#define CLOSE(a,b,delta) (ABS((a)-(b)) < (delta))

/* Note: string pointers are (char*), not (u8*) or (s8*) */
typedef unsigned char       u8;   /* unsigned 8 bit   0 to 255         */
typedef signed char         s8;   /* signed 8 bit     -128 to 127      */
typedef unsigned short int  u16;  /* unsigned 16 bit  0 to 65535       */
typedef signed short int    s16;  /* signed 16 bit    -32768 to 32767  */
typedef unsigned int        u32;  /* unsigned 32 bit  0 to 4294967295  */
typedef signed int          s32;  /* signed 32 bit    -2147483648 to
                                                       2147483647      */
typedef unsigned long int   u64;
typedef signed long int     s64;

typedef float               r32;  /* floating point 32 bit  
                                     1.18E-38 to 3.4E38   */
typedef double              r64;  /* floating point 64 bit  
                                     1.18E-308 to 3.4E308 */

typedef double real;

#endif /* _BASE_H */

