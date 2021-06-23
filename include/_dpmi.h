/****************************************************************************
; *                                                                         *
; *     File        : _dpmi.h
; *                                                                         *
; *     Description : DPMI Interface
; *                                                                         *
; *     Copyright © Realtech VR 1995,2000
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
#ifndef __DPMI_HH
#define __DPMI_HH

#include "_std.h"
#define PM_RealToLinear(x)   ((((ulong)x>>16)&(0x0000FFFF))<<4)+((ulong)x & 0x0000FFFF)

typedef struct _pm_dpmiInfo {
    unsigned char major;
    unsigned char minor;
    unsigned char realmode,paging;
    unsigned char cpu;
    unsigned char master_pic;
    unsigned char slave_pic;
}PM_DPMIINFO;

typedef struct _pm_buffer {
	unsigned int len,sel,off,rseg,roff;
}PM_Buffer;

#ifdef __WATCOM__
	void PM_memsetf(unsigned long dst_s,unsigned long dst_o,ulong color,unsigned long n);
	#pragma aux PM_memsetf = \
		"push es"\
		"mov es,bx"\
		"push ecx"\
		"shr ecx,2"\
		"rep stosd"\
		"pop ecx"\
		"and ecx,3"\
		"rep stosb"\
		"pop es"\
		parm [ebx] [edi] [eax] [ecx];
	void PM_memcpyfn(unsigned long dst_s,unsigned long dst_o,void *src,unsigned long n);
	#pragma aux PM_memcpyfn = \
		"push es"\
		"mov es,ax"\
		"mov eax,ecx"\
		"shr ecx,2"\
		"rep movsd"\
		"mov ecx,eax"\
		"and ecx,3"\
		"rep movsb"\
		"pop es"\
		parm [eax] [edi] [esi] [ecx];
	void PM_memcpynf(void *dst,unsigned long src_s,unsigned long src_o,unsigned long n);
	#pragma aux PM_memcpynf = \
		"push ds"\
		"mov ds,ax"\
		"mov eax,ecx"\
		"shr ecx,2"\
		"rep movsd"\
		"mov ecx,eax"\
		"and ecx,3"\
		"rep movsb"\
		"pop ds"\
		parm [edi] [eax] [esi] [ecx];
#else
       // DJGPP Conventions
       #define PM_memcpynf(dst,src_s,src_o,n) dosmemget(src_s, n, dst);
       #define PM_memcpyfn(dst_s,dst_o,src,n) dosmemput(src, n, dst_s);
#endif

#ifdef __cplusplus
extern "C" {
#endif

    ulong  PM_memoryLeft(void); // Return the amount of free memory
    void   PM_readSpecification(PM_DPMIINFO *Dps); // Return PM specifications
    void   PM_callES(uchar _dosint, __dpmi_regs *regs, void *buffer, int size);
           // simulate a real mode interrupt. copy into ES:00 a buffer and a size.
           // see vbecore.c for more samples

    void   RM_Release(void);
    int    RM_Initialize(void);
    
    extern PM_Buffer   PMB;

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __DPMI_HH

