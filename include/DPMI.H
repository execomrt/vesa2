;/************************************************************************
; *
; *     File        :   DPMI.H
; *
; *     Description :   Header for DPMI functions with Watcom
; *
; *     Copyright (C) 1995 Realtech
; *
; ***********************************************************************/
#ifndef __DPMIHT
#define __DPMIHT
struct _RMWORDREGS {
    unsigned short ax, bx, cx, dx, si, di, cflag, flags;
};
struct _RMBYTEREGS {
    unsigned char   al, ah, bl, bh, cl, ch, dl, dh;
};
typedef union {
    struct  _RMWORDREGS x;
    struct  _RMBYTEREGS h;
} RMREGS;
typedef struct {
    unsigned short  es;
    unsigned short  cs;
    unsigned short  ss;
    unsigned short  ds;
} RMSREGS;
void PM_memsetf(unsigned long dst_s,unsigned long dst_o,unsigned long color,unsigned long n);
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
extern unsigned int PMBuf_len,PMBuf_sel,PMBuf_rseg,PMBuf_roff,PMBuf_off;
#ifdef __cplusplus
extern "C" {
    #endif
    void PM_allocRealSeg(unsigned int size,unsigned int *rel,unsigned int *r_seg);
    void PM_freeRealSeg(unsigned sel);
    int PM_int86(int intno,RMREGS *in,RMREGS *out);
    int PM_int86x(int intno,RMREGS *in,RMREGS *out,RMSREGS *sregs);
    int PM_allocSelector(void);
    int PM_mapPhysicalToLinear(long phyAddr,long limit,unsigned long *newx);
    void PM_setSelectorBase(int sel,long linAddr);
    void PM_setSelectorLimit(int sel,long limit);
    void deinitRMbuf(void);
    void initRMbuf(void);
    #ifdef __cplusplus
}
#endif
#endif
