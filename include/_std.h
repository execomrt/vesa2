/************************************************************************
; *
; *     File        : _STD.H
; *
; *     Description : Low level definitions and debug support               *
; *                                                                         *
; *     Copyright (C) 1997 Realtech                                         *
; *                                                                         *
; *     Permission to use, copy, modify, distribute and sell this software  *
; *     and its documentation for any purpose is hereby granted without fee,*
; *     provided that the above copyright notice appear in all copies and   *
; *     that both that copyright notice and this permission notice appear   *
; *     in supporting documentation.  Realtech makes no representations     *
; *     about the suitability of this software for any purpose.             *
; *     It is provided "as is" without express or implied warranty.         *
; *                                                                         *
; ***************************************************************************/
#ifndef __STD_HH
#define __STD_HH

#define __C32__
#ifndef _GNU__
 #define __WATCOM__
#endif
#define _PC

#pragma pack(1)

#if !defined __USE_MISC || defined _SUPPORT_DEFS_H
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned int uint;
#endif
typedef unsigned char  uchar;

enum {
    false,
    true
};

#ifdef __WATCOM__
  #define CALLING_C  cdecl
  #define INTERRUPT_F interrupt
#else
  #define CALLING_C
  #define INTERRUPT_F
#endif
#define LOCAL      static
#ifndef random
  #define random(a)   (rand()%(a))
#endif

#ifndef __extern_c
 #ifdef __cplusplus
    #define __extern_c            extern "C" {
    #define __end_extern_c        }
 #else
    #define __extern_c
    #define __end_extern_c
 #endif
#endif


#endif
