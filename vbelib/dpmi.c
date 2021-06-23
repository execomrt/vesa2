/***************************************************************************
; *                                                                         *
; *     File        : DPMI.C                                                *
; *                                                                         *
; *     Description : DPMI Functions and RM/PM interface                    *
; *                                                                         *
; *     Copyright (C) 1994,97 Realtech                                      *
; *                                                                         *
; *     Permission to use, copy, modify, distribute and sell this software  *
; *     and its documentation for any purpose is hereby granted without fee,*
; *     provided that the above copyright notice appear in all copies and   *
; *     that both that copyright notice and this permission notice appear   *
; *     in supporting documentation.  Realtech makes no representations     *
; *     about the suitability of this software for any purpose.             *
; *     It is provided "as is" without express or implied warranty.         *
; *                                                                        *
; *************************************************************************/
#include <dos.h>
#include <assert.h>
#include <string.h>
#include "_std.h"

#ifdef __WATCOMC__
  #include <i86.h>
  #include "_wdpmi.h"
#else
  #include <pc.h>
  #include <dpmi.h>
  #include <go32.h>
  #include <sys/nearptr.h>
  #include <crt0.h>
  int _crt0_startup_flags = _CRT0_FLAG_NEARPTR | _CRT0_FLAG_NONMOVE_SBRK;
#endif

#include "_dpmi.h"

PM_Buffer PMB={4096,0,0,0,0};

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void RM_Release(void)
*
* DESCRIPTION : Release RM to PM interface
*
*/
void RM_Release(void)
{
    __dpmi_free_dos_memory(PMB.sel);
    PMB.sel = 0;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : int RM_Initialize(void)
*
* DESCRIPTION : Initialize RM to PM interface
*
*/
int RM_Initialize(void)
{
    __dpmi_allocate_dos_memory(PMB.len,(int*)(&PMB.sel));
    PMB.rseg = __tb;    
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void PM_callES(uchar _dosint,__dpmi_regs *regs,void *buffer,int size)
*
* DESCRIPTION : Simulate an real mode interruption. data is copied in es:xxx
*
*/
void PM_callES(uchar _dosint,__dpmi_regs *regs,void *buffer,int size)
{
    #ifdef __WATCOMC__
		assert(PMB.sel!=0);
		regs->x.es = PMB.rseg;
		PM_memcpyfn(PMB.sel,0,buffer,size);
		__dpmi_int(_dosint,regs);
		PM_memcpynf(buffer,PMB.sel,0,size);
    #else
		regs->x.es = __tb/16;
		dosmemput(buffer,size,__tb);
		__dpmi_int(_dosint,regs);
		dosmemget(__tb,size,buffer);
    #endif
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void PM_readSpecification(PM_DPMIInfo *Dps)
*
* DESCRIPTION : DPMI informations (CPU, Realmode etc...)
*
*/

void PM_readSpecification(PM_DPMIINFO *Dps)
{
    __dpmi_version_ret ret;
    __dpmi_get_version(&ret);
    Dps->minor    =  ret.minor;
    Dps->major    =  ret.major;
    Dps->realmode =  ret.flags & 1;
    Dps->paging   = (ret.flags& 2)!=0;
    Dps->cpu      =  ret.cpu;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : ulong PM_memoryLeft(void)
*
* DESCRIPTION : Give the largest amount of memory free
*
*/
ulong PM_memoryLeft(void)
{
    __dpmi_free_mem_info MemInfo;
    __dpmi_get_free_memory_information(&MemInfo);
    return MemInfo.largest_available_free_block_in_bytes;
}
