#define SAFE_VESA_IMPLEMENTATION
#define VESA2_LFB_IMPLEMENTATION

/***************************************************************************
; *                                                                         *
; *     File        :   vbecore.c
; *                                                                         *
; *     Description :   VESA 3 Interface                                    *
; *                                                                         *
; *     Copyright (C) 1994,2000 Realtech                                    *
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
#include <stdio.h>
#include <string.h>
#include <dos.h>

#include "_std.h"
#include "_vesa.h"
#include "_wdpmi.h"
#include "_dpmi.h"

#ifdef DJGPP
    #include <sys/nearptr.h>
    #include <crt0.h>
#endif

typedef struct _vesa_system {   
    void*           lpFrameBuffer;
    uchar*          lpBackBuffer;
    uchar*          lpFrontBuffer;
    unsigned        dwCaps;

    ulong           dwOffset;
    ulong           dwLinAddr;

    short           wSelAddr;
    ushort          wBankShift;
}VBE_System;

VBE_CoreInfoBlock   VBE_Core;
VBE_ModeInfoBlock   VBE_mode;
static VBE_System    VESA;

#define vbeMemPK       4
#define vbeUseLFB      0x4000

// Mode Attributes caps
#define vbeMdAvailable 0x0001
#define vbeMdColorMode 0x0008
#define vbeMdGraphMode 0x0010
#define vbeMdNonbanked 0x0040
#define vbeMdLinear    0x0080

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void *_vbe_get_lfb(long physaddr)
*
* DESCRIPTION : Computes LFB, remaps video
*
*/
static int _vbe_get_lfb(long physaddr)
{
    __dpmi_meminfo info;
    if (VESA.dwLinAddr)
    {
        info.address = VESA.dwLinAddr;
        __dpmi_free_physical_address_mapping(&info);
        VESA.dwLinAddr = 0;
    }
    #ifdef DJGPP
        info.address = physaddr;
        info.size    = LIMIT4G;
        if (__dpmi_physical_address_mapping(&info)!=-1) // carry clear
        {
            VESA.dwLinAddr = info.address;
            VESA.lpFrameBuffer   = (uchar*)VESA.dwLinAddr; //<<= Pointer to Video
            VESA.dwCaps |= VESACAPS_ISLINEAR;         
            return 0;
        }
    #else
        #define LIMIT4G 4096L*1024L-1L
        if (VESA.wSelAddr)
        {
            __dpmi_free_ldt_descriptor(VESA.wSelAddr);
            VESA.wSelAddr = 0;
        }
        {
           int ldt;
           ldt = __dpmi_allocate_ldt_descriptors(1);
           if (ldt == 0x7fffffff)
           {
              return VBEERR_ALLOCLDTFAILED; 
           }
           VESA.wSelAddr = ldt;
        }
        if (__dpmi_set_descriptor_access_rights(VESA.wSelAddr,0x8092))
        {           
            return VBEERR_ACCESSRIGHTFAILED;        
        }
        info.address = physaddr;
        info.size    = LIMIT4G;
        if (!__dpmi_physical_address_mapping(&info)) // carry clear
        {
            VESA.dwLinAddr = info.handle;
            __dpmi_set_segment_base_address(VESA.wSelAddr,VESA.dwLinAddr);
            __dpmi_set_segment_limit(VESA.wSelAddr,LIMIT4G);
            __dpmi_get_segment_base_address(VESA.wSelAddr,&VESA.dwLinAddr);
            VESA.lpFrameBuffer = (uchar*)VESA.dwLinAddr; //<<= Pointer to Video
            VESA.dwCaps |= VESACAPS_LINEAR;           
            return VBEERR_NOERR;
        }
    #endif  
    VESA.dwCaps = 0;
    if (VESA.wSelAddr)
    {
       __dpmi_free_ldt_descriptor(VESA.wSelAddr);
       VESA.wSelAddr = 0;
    }
    return VBEERR_PHYSICALADDRMAPFAILED;
}

static int _vbe_setdisplaymode(int mode)
{
    __dpmi_regs regs;
    regs.x.ax = 0x4F02;
    regs.x.bx = mode;
    __dpmi_int(0x10,&regs);
    if (regs.x.ax==0x004f)
    {
        VBE_SetScanlineLength(VBE_mode.XResolution);
        VESA.dwCaps &=~ VESACAPS_PAGECOUNT;
        VESA.lpFrontBuffer = 0;
        VESA.lpBackBuffer = 0;
        return VBEERR_NOERR;
    }
    return VBEERR_SETDISPLAYMODEFAILED;
}

static int _vbe_initialize_lfb(int bMustBeLinear)
{
    VESA.wBankShift = 0;
    VESA.dwCaps      = 0;
    while ((64>>VESA.wBankShift)!=VBE_mode.WinGranularity) VESA.wBankShift++;
    if ((VBE_mode.ModeAttributes & vbeMdAvailable)
    && (VBE_mode.ModeAttributes & vbeMdLinear || (!bMustBeLinear)))
    {
        int err = _vbe_get_lfb(VBE_mode.PhysicBasePtr);
        if (err!=VBEERR_NOERR) return err;
    }
    else return VBEERR_NONLINEAR;
    return VBEERR_NOERR;
}

static int _vbe_read_vbe_mode(VBE_ModeInfoBlock *dest, VBE_MODE _mode)
{
    __dpmi_regs regs;
    regs.x.ax = 0x4F01;
    regs.x.cx = _mode;
    regs.x.di = 0;
    memset(dest,0,sizeof(VBE_ModeInfoBlock));
    PM_callES(0x10,&regs, dest, sizeof(VBE_ModeInfoBlock));
    return (regs.x.ax == 0x004F) ? VBEERR_NOERR : VBEERR_CANTREADMODEINFO;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : int VBE_FindDisplayMode(int width,int height,int bbp)
*
* DESCRIPTION : Search for a mode
*
*/
VBE_MODE VBE_FindDisplayMode(int width,int height,int bpp)
{
    const ushort *md = VBE_Core.VideoModePtr;
    VBE_ModeInfoBlock modeblck;
    while (*md!=0xffff)
    {
        if (_vbe_read_vbe_mode(&modeblck,*md)==VBEERR_NOERR)
        {
            if (modeblck.YResolution==height)
            if (modeblck.XResolution==width)
            {
                switch(bpp) {
                    case 15: // Matrox Bug
                    case 16:
                        if (bpp==modeblck.BlueMaskSize + modeblck.GreenMaskSize + modeblck.RedMaskSize) return *md;
                    break;
                    default:
                        if (bpp==modeblck.BitsPerPixel) return *md;
                    break;
                }
            }
        }
        md++;
    }
    return 0;
}

VBE_RESULT VBE_EnumDisplayModes(VBE_ENUMDISPLAYCALLBACK callback, void *context)
{
    const ushort *md = VBE_Core.VideoModePtr;
    VBE_ModeInfoBlock modeblck;
    while (*md!=0xffff)
    {
        VBE_MODE m = (VBE_MODE)(*md)&0xffff;        
        if (_vbe_read_vbe_mode(&modeblck, m)==VBEERR_NOERR)
        {
            if (callback(&modeblck, m, context)==0) return VBEERR_NOERR;
        }
        md++;
    }
    return VBEERR_NOERR;
}

#ifdef SAFE_VESA_IMPLEMENTATION
static ushort *vbe_alternate_table()
{
   int i;
   VBE_ModeInfoBlock modeblck;
   static ushort v[0x200];
   ushort *vv = v;
   memset(v,0xff,0x200);
   for (i=0x100;i<0x200;i++) // Scan all modes from 0x100 to 0x1ff
   {
      if (_vbe_read_vbe_mode(&modeblck, i)==VBEERR_NOERR)
      {
         *vv = i;
         vv++;
      }
   }
   return v;
}
#endif

VBE_RESULT VBE_Initialize(void)
{
    __dpmi_regs regs;
    memset(&VBE_Core,0,sizeof(VBE_CoreInfoBlock));
    strncpy(VBE_Core.VESASignature,"VBE2",4);
    regs.x.ax = 0x4F00;
    regs.x.di = 0;
    PM_callES(0x10,&regs, &VBE_Core, sizeof(VBE_CoreInfoBlock));
    if (regs.x.ax != 0x004F) return 0;
    if (strncmp(VBE_Core.VESASignature,"VESA",4) != 0)
    {
        memset(&VBE_Core,0,sizeof(VBE_CoreInfoBlock));
        return VBEERR_NOVESADETECTED;
    }
    VBE_Core.OEMStringPtr      = (const char  *)PM_RealToLinear((ulong)VBE_Core.OEMStringPtr);
    VBE_Core.OEMVendorNamePtr  = (const char  *)PM_RealToLinear((ulong)VBE_Core.OEMVendorNamePtr);    
    VBE_Core.OEMProductNamePtr = (const char  *)PM_RealToLinear((ulong)VBE_Core.OEMProductNamePtr);
    VBE_Core.OEMProductRevPtr  = (const char  *)PM_RealToLinear((ulong)VBE_Core.OEMProductRevPtr);

	// Since VBE_Core.VideoModePtr is NOT reliable (bad implementation on ATI 128), we uses our own alternate table.
#ifdef SAFE_VESA_IMPLEMENTATION
    VBE_Core.VideoModePtr = (const ushort*)vbe_alternate_table(); 
#else
	VBE_Core.VideoModePtr = PM_RealToLinear((ulong)VBE_Core.VideoModePtr);
#endif
    return VBEERR_NOERR;
}

void VBE_Release(void)
{      
    if (VESA.dwLinAddr)
    {
       __dpmi_meminfo info;
        info.address = VESA.dwLinAddr;
        __dpmi_free_physical_address_mapping(&info);
        VESA.dwLinAddr = 0;
    }
    if (VESA.wSelAddr)
    {
       __dpmi_free_ldt_descriptor(VESA.wSelAddr);
        VESA.wSelAddr = 0;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : int VBE_SetDisplayMode(VBEMODE _mode)
*
* DESCRIPTION : Check mode, computes LFB and sets mode
*
*/
VBE_RESULT VBE_SetDisplayMode(VBE_MODE _mode)
{
    #ifdef VESA2_LFB_IMPLEMENTATION
    int bMustBeLinear = 1; // video mode must be linear.
    #else
    int bMustBeLinear = 0; // authorize VESA 1.2 modes (banked modes)
    #endif
    VBE_RESULT hr;
    hr = _vbe_read_vbe_mode(&VBE_mode, _mode); if (hr!=VBEERR_NOERR) return hr;
    hr = _vbe_initialize_lfb(bMustBeLinear); if (hr!=VBEERR_NOERR) return hr;
    return _vbe_setdisplaymode( (VESA.dwCaps & VESACAPS_LINEAR) 
                                   ? _mode | vbeUseLFB 
                                   : _mode);                                
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : bool VBE_SetDAC(int mode)
*
* DESCRIPTION : Set new DAC mode
*
*/
VBE_RESULT VBE_SetDAC(int mode)
{
    if (mode == VBE_PAL6BITDAC)
    {
        __dpmi_regs  regs;
        regs.x.ax = 0x4F08;
        regs.x.bx = 0x0600;
        __dpmi_int(0x10,&regs);
        if (regs.x.ax != 0x004F)
        return true;
        if (regs.h.bh != 6)
        return false;
        return true;
    }
    if (mode == VBE_PAL8BITDAC)
    {
        __dpmi_regs  regs;
        regs.x.ax = 0x4F08;         /* Set DAC service                      */
        regs.x.bx = 0x0800;         /* BH := 8, BL := 0 (set DAC width)     */
        __dpmi_int(0x10,&regs);
        if (regs.x.ax != 0x004F)
        return false;               /* Function failed, no wide dac         */
        if (regs.h.bh == 6)
        return false;
        regs.x.ax = 0x4F08;
        regs.x.bx = 0x0001;         /* Get DAC width (should now be 8)      */
        __dpmi_int(0x10,&regs);
        if (regs.x.ax != 0x004F)
        return false;
        if (regs.h.bh != 8)
        return false;
        return true;
    }
    return false;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void CALLING_C VBE_SetPalette(ulong a, ulong b, const void * pal)
*
* DESCRIPTION : Set palette (using directly in port is faster)
*
*/
void CALLING_C VBE_SetPalette(ulong start, ulong numEntries, const void * pal)
{
    __dpmi_regs  regs;
    regs.x.ax = 0x1012;
    regs.x.bx = start;
    regs.x.cx = numEntries;
    regs.x.es = PMB.rseg;
    regs.x.dx = PMB.roff;
    PM_memcpyfn(PMB.sel,PMB.off, (void*)pal, (numEntries-1) * 3);
    __dpmi_int(0x10, &regs);
    return ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void VBE_GetPalette(int start, int numEntries, uchar *palbuf)
*
* DESCRIPTION : Get current palette
*
*/
VBE_RESULT VBE_GetPalette(int start, int numEntries, uchar *pal)
{
    __dpmi_regs  regs;
    regs.x.ax = 0x1017;
    regs.x.bx = start;
    regs.x.cx = numEntries;
    regs.x.es = PMB.rseg;
    regs.x.dx = PMB.roff;
    __dpmi_int(0x10, &regs);
    PM_memcpynf(pal, PMB.sel, PMB.off, numEntries * 3);
    return VBEERR_NOERR;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C VBE_Flip(void)
*
* DESCRIPTION : page flipping sample code
*
*/
void CALLING_C VBE_Flip(void)
{
    __dpmi_regs  regs;

    VESA.dwCaps ^= VESACAPS_PAGECOUNT;

    regs.x.ax = 0x4F07;
    regs.x.bx = 0x0000;  //00h:set 01h:get 80h:during Vertical (not on matrox)
    regs.x.cx = 0;
    regs.x.dx = VESA.dwCaps & VESACAPS_PAGECOUNT  ? VBE_mode.YResolution : 0;

    __dpmi_int(0x10,&regs);
    
    return;
}

uchar *VBE_LockSurface()
{
    ulong offsetFront, offsetBack;
    
    offsetFront = VESA.dwCaps & VESACAPS_PAGECOUNT ? VBE_mode.YResolution : 0;
    offsetBack  = VESA.dwCaps & VESACAPS_PAGECOUNT ? 0 : VBE_mode.YResolution;
   
    VESA.lpFrontBuffer = (uchar*)VESA.lpFrameBuffer + offsetFront * (ulong)VBE_mode.BytesPerLine
    #ifdef DJGPP
      + __djgpp_conventional_base
    #endif
    ;

    VESA.lpBackBuffer  = (uchar*)VESA.lpFrameBuffer + offsetBack * (ulong)VBE_mode.BytesPerLine;
    return VESA.lpBackBuffer
    #ifdef DJGPP
      + __djgpp_conventional_base
    #endif
    ;
}
void VBE_LockSurfaceAlt(uchar **front, uchar **back, long *pitch)
{
    VBE_LockSurface();
    if (pitch) *pitch = VBE_mode.BytesPerLine;
    if (back)  *back  = VESA.lpBackBuffer;
    if (front) *front = VESA.lpFrontBuffer;
}

void VBE_UnlockSurface()
{     
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void VESASetScanLineLength(long new_length)
*
* DESCRIPTION : Change scanline length
*
*/
VBE_RESULT VBE_SetScanlineLength(long new_length)
{
    __dpmi_regs  regs;
    regs.x.ax = 0x4F06;
    regs.x.bx = 0x0000;        //Set Scanline
    regs.x.cx = new_length;
    regs.x.dx = 0x0000;
    __dpmi_int(0x10,&regs);
    if (regs.x.ax!=2)
    {
        regs.x.ax = 0x4F06;
        regs.x.bx = 0x0001;   // Get Scanline
        regs.x.dx = 0x0000;
        __dpmi_int(0x10,&regs);
        if (regs.x.bx) VBE_mode.BytesPerLine = regs.x.bx;
    }
    return VBEERR_NOERR;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void VBE_set_bank(int bank)
*
* DESCRIPTION : Set current bank (for non-linear mode)
*
*/
void VBE_SetBank(int bank)
{
    __dpmi_regs regs;
    regs.x.ax = 0x4f05;
    regs.x.bx = 0;
    regs.x.dx = bank << VESA.wBankShift;
    __dpmi_int(0x10,&regs);
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void VBE_set_dwOffset(long x,long y)
*
* DESCRIPTION : Change video dwOffset
*
*/
void VBE_SetOffset(long x,long y)
{
    __dpmi_regs regs;
    regs.x.ax = 0x4F07;
    regs.x.bx = 0x0000;
    regs.x.cx = x;
    regs.x.dx = y;
    __dpmi_int(0x10,&regs);
    return;
}

typedef struct _vbe_pm_table {
    ushort wOffSet_wPos;
    ushort wOffSet_dStart;
    ushort wOffSet_paletteData;
    ushort wOffSet_PortTable;
}VBE_PM_Table;

void VBE_GetPMI()
{
    __dpmi_regs regs;
    regs.x.ax = 0x4F0a;
    regs.x.bx = 0x0000;
    __dpmi_int(0x10,&regs);
    if (regs.x.ax==0x004f)
    {         
       // Success
       // regs.x.cx = length of the table
       // es:di = data
    }
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : ushort VBEPM_Detect(void)
*
* DESCRIPTION : Power Management functions
*
*/
VBE_RESULT VBEPM_Initialize(void)
{
    __dpmi_regs regs;
    regs.x.ax = 0x4f10;
    regs.x.bx = 0x0;
    regs.x.di = 0;
    __dpmi_int(0x10,&regs);
    if (regs.h.al==0x4f) return regs.x.bx;
    return -1;
}
void VBEPM_SetState(int mode)
{
    __dpmi_regs regs;
    regs.x.ax = 0x4f10;
    regs.h.bl = 0x01;
    regs.h.bh = mode<<1;
    __dpmi_int(0x10,&regs);
    return;
}
int VBEPM_GetState(void)
{
    __dpmi_regs regs;
    regs.x.ax = 0x4f10;
    regs.h.bl = 0x02;
    __dpmi_int(0x10,&regs);
    if (regs.h.al==0x4f) return regs.h.bh;
    return -1;
}

VBE_RESULT VGA_SetDisplayMode(int a)
{
    __dpmi_regs regs;
    regs.x.ax = a;
    __dpmi_int(0x10,&regs);
    return VBEERR_NOERR;
}

void VGA_WaitVBL()
{
    return;
}

ulong VBE_GetCaps()
{
    return VESA.dwCaps;
}

const char *VBE_GetStringErr(VBE_RESULT err)
{
   switch(err) {
       case VBEERR_NOERR                : return "No error.";
       case VBEERR_SETDISPLAYMODEFAILED : return "Failed to setup the requested display mode.";
       case VBEERR_ACCESSRIGHTFAILED    : return "Protected mode access rights has been refused from DPMI manager.";
       case VBEERR_PHYSICALADDRMAPFAILED: return "Physical remapping has failed.";
       case VBEERR_CANTREADMODEINFO     : return "Couldn't query display mode informations.";
       case VBEERR_NOVESADETECTED       : return "VBE interface hasn't been detected.";
       case VBEERR_ALLOCLDTFAILED       : return "Protected mode LDT allocation failed.";
       case VBEERR_NONLINEAR:             return "Non linear or unavailable display mode";
   }
   return "Unknown error.";
}

