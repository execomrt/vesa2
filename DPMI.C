;/*************************************************************************
; *
; *     File        : DPMI.C
; *
; *     Description : DPMI Functions
; *
; *     Copyright (C) 1995 RealTech
; *
; ************************************************************************/
#include <dos.h>
#include <string.h>
#include <conio.h>
#include <i86.h>
#include "_dpmi.h"
unsigned int PMBuf_len  = 1024,
PMBuf_sel  =    0,
PMBuf_off  =    0,
PMBuf_rseg =    0,
PMBuf_roff =    0;
#pragma pack(1)
typedef struct {
    long edi;
    long esi;
    long ebp;
    long reserved;
    long ebx;
    long edx;
    long ecx;
    long eax;
    short flags;
    short es,ds,fs,gs,ip,cs,sp,ss;
}_RMREGS;
#define pragma pack()
#define IN(reg)  rmregs.e##reg = in->x.reg
#define OUT(reg) out->x.reg = (short)rmregs.e##reg
/*------------------------------------------------------------------------
*
* PROTOTYPE  :void PM_allocRealSeg(unsigned int size,unsigned int *rel,unsigned int *r_seg)
*
* DESCRIPTION :
*
*/
void PM_allocRealSeg(unsigned int size,unsigned int *rel,unsigned int *r_seg)
{
    union REGS r;
    r.w.ax = 0x100;
    r.w.bx = (short) ((size+0xF)>>4);
    int386(0x31,&r,&r);
    *rel = r.w.dx;
    *r_seg = r.w.ax;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :void PM_freeRealSeg(unsigned sel)
*
* DESCRIPTION :
*
*/
void PM_freeRealSeg(unsigned sel)
{
    union REGS r;
    r.w.ax = 0x101;
    r.w.bx = (short) sel;
    int386(0x31,&r,&r);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :int PM_int86(int intno,RMREGS *in,RMREGS *out)
*
* DESCRIPTION :
*
*/
int PM_int86(int intno,RMREGS *in,RMREGS *out)
{
    _RMREGS rmregs;
    union REGS r;
    struct SREGS sr;
    memset(&rmregs,0,sizeof(rmregs));
    IN(ax);IN(bx);IN(cx);IN(dx);IN(si);IN(di);
    segread(&sr);
    r.w.ax  = 0x300;
    r.h.bl  = (short)intno;
    r.h.bh  = 0;
    r.w.cx  = 0;
    sr.es   = sr.ds;
    r.x.edi = (unsigned)&rmregs;
    int386x(0x31,&r,&r,&sr);
    OUT(ax);OUT(bx);OUT(cx);OUT(dx);OUT(si);OUT(di);
    out->x.cflag = (short) (rmregs.flags & 0x1);
    return out->x.ax;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :int PM_int86x(int intno,RMREGS *in,RMREGS *out,RMSREGS *sregs)
*
* DESCRIPTION :
*
*/
int PM_int86x(int intno,RMREGS *in,RMREGS *out,RMSREGS *sregs)
{
    _RMREGS rmregs;
    union REGS r;
    struct SREGS sr;
    memset(&rmregs,0,sizeof(rmregs));
    IN(ax);IN(bx);IN(cx);IN(dx);IN(si);IN(di);
    rmregs.es = sregs->es;
    rmregs.ds = sregs->ds;
    segread(&sr);
    r.w.ax  = 0x300;
    r.h.bl  = intno;
    r.h.bh  = 0;
    r.w.cx  = 0;
    sr.es   = sr.ds;
    r.x.edi = (unsigned)&rmregs;
    int386x(0x31,&r,&r,&sr);
    OUT(ax);OUT(bx);OUT(cx);OUT(dx);OUT(si);OUT(di);
    sregs->es = rmregs.es;
    sregs->cs = rmregs.cs;
    sregs->ss = rmregs.ss;
    sregs->ds = rmregs.ds;
    out->x.cflag = rmregs.flags & 0x1;
    return out->x.ax;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :int PM_allocSelector(void)
*
* DESCRIPTION :
*
*/
int PM_allocSelector(void)
{
    int sel;
    union REGS r;
    r.w.ax = 0;
    r.w.cx = 1;
    int386(0x31,&r,&r);
    if (r.x.cflag) return -1;
    sel    = r.w.ax;
    r.w.ax = 9;
    r.w.bx = sel;
    r.w.cx = 0x8092;
    int386(0x31,&r,&r);
    if (r.x.cflag) return -2;
    return sel;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :int PM_mapPhysicalToLinear(long phyAddr,long limit,unsigned long *newx)
*
* DESCRIPTION :
*
*/
int PM_mapPhysicalToLinear(long phyAddr,long limit,unsigned long *newx)
{
    union REGS r;
    r.w.ax = 0x800;
    r.w.bx = (short) (phyAddr >> 16);
    r.w.cx = (short) (phyAddr & 0xFFFF);
    r.w.si = (short) (limit   >>16 );
    r.w.di = (short) (limit   & 0xFFFF);
    int386(0x31,&r,&r);
    *newx = ((unsigned long)r.w.bx << 16) + (unsigned long)r.w.cx;
    return r.x.cflag;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :void PM_setSelectorBase(int sel,long linAddr)
*
* DESCRIPTION :
*
*/
void PM_setSelectorBase(int sel,long linAddr)
{
    union REGS r;
    r.w.ax = 7;
    r.w.bx = sel;
    r.w.cx = linAddr >>16;
    r.w.dx = linAddr & 0xFFFF;
    int386(0x31,&r,&r);
    if (r.x.cflag) return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :void PM_setSelectorLimit(int sel,long limit)
*
* DESCRIPTION :
*
*/
void PM_setSelectorLimit(int sel,long limit)
{
    union REGS r;
    r.w.ax = 8;
    r.w.bx = sel;
    r.w.cx = limit >> 16;
    r.w.dx = limit & 0xFFFF;
    int386(0x31,&r,&r);
    if (r.x.cflag) return;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void deinitRMbuf(void)
*
* DESCRIPTION :
*
*/
void deinitRMbuf(void)
{
    PM_freeRealSeg(PMBuf_sel);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void initRMbuf(void)
{
    if (PMBuf_sel==0)
    {
        PM_allocRealSeg(PMBuf_len,&PMBuf_sel,&PMBuf_rseg);
    }
    return;
}
