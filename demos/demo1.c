/****************************************************************************
; *                                                                         *
; *     File        : demo1.c
; *                                                                         *
; *     Description : VESA 2.0 Tests
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

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <time.h>

#include "_std.h"
#include "_wdpmi.h"
#include "_dpmi.h"
#include "_vesa.h"

// ======================================================== First Demo, display a full screen image
int static DrawJulia(int DEEP,int MAXITER,float xmin,float xmax,float ymin,float ymax,float a,float b)
{
    float LX = abs(xmax) + abs(xmin), LY = abs(ymax) + abs(ymin),cx,cy,ccy;
    float dLY = LY/(float)VBE_mode.YResolution;
    float dLX = LX/(float)VBE_mode.XResolution;
    
    long delta = (VBE_mode.BytesPerLine/2) - VBE_mode.XResolution;
    long y,x,k;
    ushort *video = 0;
    VBE_LockSurfaceAlt( (uchar**)&video, 0, 0); // Get front buffer.
    
    if (!video) return -1;
    for (y = 0;y<VBE_mode.YResolution;y++)
    {
        if (kbhit()) return 1; // Abort by user
        ccy = ymin + (float)y * dLY;
        for (x = 0;x<VBE_mode.XResolution; x++,video++)
        {
            float cx2,cy2;
            cx = xmin + (float)x * dLX;
            cy = ccy;
            k = DEEP;
            cx2 = cx * cx;
            cy2 = cy * cy;
            do
            {
                float x1 = cx2 - cy2 - a;
                cy = 2. * cx * cy - b;
                cx = x1;
                k--;
                cx2 = cx*cx;
                cy2 = cy*cy;
            }while ( (cx2 + cy2 <= MAXITER) && (k!=0));
            *video = RGBMask((x<<8)/VBE_mode.XResolution,
                             (y<<8)/VBE_mode.YResolution,
                             k<<3);
        }
        video+=delta;
    }

    VBE_UnlockSurface();
    // Note : we don't need to call VBE_Flip since we are working directly on the front buffer.
    return 0;
}

void static Demo1(void)
{
    long t = clock();
    int success = DrawJulia(32,500,-2.,2,-1,1,.777,.166);
    t = clock()-t;
    if (success<=0)
    getch();
    VGA_SetDisplayMode(0x3);
    if (success<0) printf("Error when accessing frame buffer\n");
    else printf("Rendering time = %ld ms\n",(t*1000L)/CLK_TCK);
}

// ======================================================== main program
main()
{
    int hr;        
    printf("\n\nRealtech VESA Kit v3.0 (C) Copyright Realtech 1997,2000\nhttp://www.realtech.scene.org\n\n");
    RM_Initialize();
    hr = VBE_Initialize();
    if (hr == VBEERR_NOERR)
    {
        VBE_MODE mode = VBE_FindDisplayMode(640, 480, 16);
        if (!mode)
        {
            printf("couldn't find desired mode\n");
            return -1;
        }
        hr = VBE_SetDisplayMode(mode);
        if (hr==VBEERR_NOERR)
        {
            Demo1();
        }
        else
        {
            VGA_SetDisplayMode(0x3);
            printf("Couldn't initialize VBE Mode %xh (%s)\n",mode, VBE_GetStringErr(hr));
            getch();
        }
        VBE_Release();
        RM_Release();
    }
    else
    {
        printf("Couldn't set up VBE (%s)\n",VBE_GetStringErr(hr));
    }
    return 0;
}
