;/*************************************************************************
; *
; *     File        : TESTVESA.C
; *
; *     Description : Test Vesa mode - Initialize mode 640x480x256 in LFB
; *                   Poke some pixels in Linear Frame buffer
; *     Copyright (C) 1995 RealTech
; *
; ************************************************************************/
#include <conio.h>
#include <graph.h>
#include <stdlib.h>
#include <stdio.h>
#include "_dpmi.h"
#include "_vesa2.h"
#define random(x) (rand()%x)
unsigned long MEM_SC,width_size;
unsigned char *vid_ptr;
#ifdef __cplusplus
extern "C" {
    #endif
    void cdecl psetPixel(long x, long y, unsigned long colour);
    unsigned long cdecl getPixel(long x, long y);
    void cdecl Virtual_Clear(void);
    extern unsigned long MEM_SC,width_size;
    extern unsigned char *vid_ptr;
#ifdef __cplusplus
}
#endif
void main(void)
{
    int mode = VESA_640x480;
    unsigned long cx;
    if (Get_VesaInfo(mode))
    {
	if (Init_Vesamode(mode))
	{
	    MEM_SC = modebuf.XResolution * modebuf.YResolution;
	    width_size = modebuf.BytesPerLine;
	    vid_ptr = NULL;
	    if (useLinearVESA)
	    {
		for (cx=50000;cx!=0;cx--)
		{
		    psetPixel(random(modebuf.XResolution),random(modebuf.YResolution),
		    random(256));
		}
		getch();
	    }
	} else printf("Cannot initialize mode %x\n",mode);
	_setvideomode(_TEXTC80);
	printf("Vesa Detected : version %d.%d\n",vesabuf.VESAVersion>>8,vesabuf.VESAVersion&0xff);
	printf("Linear Frame Buffer (LFB) :%s\n\n",(useLinearVESA==1) ? "yes":"no");
    } else printf("Awa ! VESA not detected !\n");
    return;
}
