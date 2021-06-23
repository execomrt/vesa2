;/************************************************************************
; *
; *     File        :   _VESA.C
; *
; *     Description :   VESA Interface
; *
; *     Copyright (C) 1994,95 Realtech
; *
; ***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include "_vesa2.h"
#include "_dpmi.h"
VgaInfoBlock vesabuf;
VesaModeInfo modebuf;
short VESAbankshift=0,VideoCardCode=0,VESA_selector=-1,VOffPage=0;
unsigned char useLinearVESA = 255,Current_VRAM_Page=1,SVGA_widedac=0;
char far *LFBPtr=NULL;
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void PM_callESDI(RMREGS *regs,void *buffer,int size)
*
* DESCRIPTION : Emule l'interruption 0x10 en mode prot‚g‚
*
*/
void PM_callESDI(RMREGS *regs,void *buffer,int size)
{
    RMSREGS sregs;
    initRMbuf();
    sregs.es = PMBuf_rseg;
    regs->x.di = 0;
    _fmemcpy(MK_FP(PMBuf_sel,0),buffer,size);
    PM_int86x(0x10,regs,regs,&sregs);
    _fmemcpy(buffer,MK_FP(PMBuf_sel,0),size);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void far *GetPtrToLFB(long physaddr)
*
* DESCRIPTION : Recupere l'adresse … remapper
*
*/
void far *GetPtrToLFB(long physaddr)
{
    int sel;
    unsigned long linAddr,limit =(4096*1024)-1;
    sel = PM_allocSelector();
    if (!PM_mapPhysicalToLinear(physaddr,limit,&linAddr))
    {
	PM_setSelectorBase(sel,linAddr);
	PM_setSelectorLimit(sel,limit);
	useLinearVESA = 1;
	return MK_FP(sel,0);
    }
    useLinearVESA = 0;
    return NULL;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : int Get_VesaInfo(int VESAmode)
*
* DESCRIPTION : Detecte si le mode est support‚ par le BIOS
*
*/
int Get_VesaInfo(int VESAmode)
{
    RMREGS regs;
    /* Get SuperVGA mode info   */
    regs.x.ax = 0x4F01;
    regs.x.cx = VESAmode;
    PM_callESDI(&regs, &modebuf, sizeof(VesaModeInfo));
    if (regs.x.ax != 0x004F) return 0;
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : int Init_Vesamode(int VESAmode)
*
* DESCRIPTION : Detecte si y a un BIOS VESA
*               Essaye ensuite d'initialiser le mode en lineaire
*               retourne la version du vesa
*/
int Init_Vesamode(int VESAmode)
{
    RMREGS regs;
    initRMbuf();
    memset(&vesabuf,0,sizeof(VgaInfoBlock));
    memset(&modebuf,0,sizeof(VesaModeInfo));
    strncpy(vesabuf.VESASignature,"VBE2",4);
    /* Get SuperVGA information */
    regs.x.ax = 0x4F00;
    PM_callESDI(&regs, &vesabuf, sizeof(VgaInfoBlock));
    if (regs.x.ax != 0x004F)
    return 0;
    if (strncmp(vesabuf.VESASignature,"VESA",4) != 0)
    {
	memset(&vesabuf,0,sizeof(VgaInfoBlock));
	useLinearVESA = 2;
	return 0;
    }
    if (!Get_VesaInfo(VESAmode)) return 0;
    /* Set SuperVGA mode        */
    regs.x.ax = 0x4F02;
    regs.x.bx = VESAmode;
    PM_int86(0x10,&regs, &regs);
    useLinearVESA = 0;
    VESAbankshift = 0;
    /* Calcul du bank shift */
    while ((64>>VESAbankshift)!=modebuf.WinGranularity) VESAbankshift++;
    VideoCardCode = 1;
    if ((modebuf.ModeAttributes & vbeMdAvailable)
    &&(modebuf.ModeAttributes & vbeMdLinear)
    &&(modebuf.MemoryModel == vbeMemPK))
    {
	LFBPtr = GetPtrToLFB(modebuf.PhysBasePtr);
	VESA_selector = FP_SEG(LFBPtr);
	if (useLinearVESA)
	{
	    regs.x.ax = 0x4F02;    /* Set SuperVGA mode        */
	    regs.x.bx = VESAmode | vbeUseLFB;
	    PM_int86(0x10,&regs, &regs);
	}
    }
    return vesabuf.VESAVersion;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : unsigned char set8BitPalette(void)
*
* DESCRIPTION : y a-t-il un DAC etendu 8Bit ?
*
*/
unsigned char set8BitPalette(void)
{
    RMREGS  regs;
    regs.x.ax = 0x4F08;         /* Set DAC service                      */
    regs.x.bx = 0x0800;         /* BH := 8, BL := 0 (set DAC width)     */
    PM_int86(0x10,&regs,&regs);
    if (regs.x.ax != 0x004F)
    return 0;           /* Function failed, no wide dac         */
    if (regs.h.bh == 6)
    return 0;
    regs.x.ax = 0x4F08;
    regs.x.bx = 0x0001;         /* Get DAC width (should now be 8)      */
    PM_int86(0x10,&regs,&regs);
    if (regs.x.ax != 0x004F)
    return 0;
    if (regs.h.bh != 8)
    return 0;
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : unsigned char set6BitPalette(void)
*
* DESCRIPTION : y a-t-il un DAC etendu 6Bit ?
*
*/
unsigned char set6BitPalette(void)
{
    RMREGS  regs;
    regs.x.ax = 0x4F08;
    regs.x.bx = 0x0600;
    PM_int86(0x10,&regs,&regs);     /* Restore to 6 bit DAC               */
    if (regs.x.ax != 0x004F)
    return 1;
    if (regs.h.bh != 6)
    return 0;
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void cdecl setVESAPalette(unsigned long a, unsigned long b, void * pal)
*
* DESCRIPTION : Change 'b' couleurs … partir de 'a' avec la table 'pal'
*
*/
void cdecl setVESAPalette(unsigned long a, unsigned long b, void * pal)
{
    RMREGS  regs;
    RMSREGS sregs;
    regs.x.ax = 0x1012;
    regs.x.bx = a;
    regs.x.cx = b;
    sregs.es  = PMBuf_rseg;
    regs.x.dx = PMBuf_roff;
    PM_memcpyfn(PMBuf_sel,PMBuf_len, pal, b * 3);
    PM_int86x(0x10, &regs,&regs, &sregs);
    /*WaitSynchro();*/
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void getVESAPalette(int start, int num, unsigned char *palbuf)
*
* DESCRIPTION : recupere la palette en cours
*
*/
void getVESAPalette(int start, int num, unsigned char *palbuf)
{
    RMREGS  regs;
    RMSREGS sregs;
    regs.x.ax = 0x1017;
    regs.x.bx = start;
    regs.x.cx = num;
    sregs.es = PMBuf_rseg;
    regs.x.dx = PMBuf_roff;
    PM_int86x(0x10, &regs,&regs, &sregs);
    PM_memcpynf(palbuf, PMBuf_sel,PMBuf_len, num * 3);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  BITBLT_xflip
*
* DESCRIPTION : Double Flipping en mode VESA 2.00
*
*/
void cdecl BITBLT_xflipVESA(void)
{
    RMREGS  regs;
    long x;
    VOffPage = modebuf.YResolution * Current_VRAM_Page;
    x = (long) VOffPage * modebuf.XResolution;
    Current_VRAM_Page^=1;
    regs.x.ax = 0x4F07;
    regs.x.bx = 0x0000;
    regs.x.cx = 0;
    regs.x.dx = modebuf.YResolution * Current_VRAM_Page;
    PM_int86(0x10,&regs,&regs);
    /*WaitSynchro();*/
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void VESASetScanLineLength(long newcx)
*
* DESCRIPTION : Change le scanline
*
*/
void VESASetScanLineLength(long newcx)
{
    RMREGS  regs;
    regs.x.ax = 0x4F06;
    regs.x.bx = 0x0000;
    regs.x.cx = newcx;
    PM_int86(0x10,&regs,&regs);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void VESASetXY (long x,long y)
*
* DESCRIPTION : Change le start adress de la video
*
*/
void VESASetXY (long x,long y)
{
    RMREGS  regs;
    regs.x.ax = 0x4F07;
    regs.x.bx = 0x0000;
    regs.x.cx = x;
    regs.x.dx = y;
    PM_int86(0x10,&regs,&regs);
    return;
}
