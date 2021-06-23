/****************************************************************************
; *                                                                         *
; *     File        : demo2.c
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

// ======================================================== Second Demo, display some random lines
#define XCHG(a,b,type) {type c;c=a;a=b;b=c;}
#define Pixel(x,y,colour) *(ushort*)(lpBackBuffer + y * VBE_mode.BytesPerLine + x*2)=colour

uchar *lpBackBuffer;

void static B_Line(long x1,long y1,long x2,long y2,ulong colour)
{
    long int d, dx, dy,
    aincr, bincr,
    xincr, yincr,
    x, y;
    if( abs(x2-x1) < abs(y2-y1) )
    {
        if( y1 > y2 )
        {
            XCHG( x1, x2, long );
            XCHG( y1, y2, long );
        }
        xincr = ( x2 > x1 ) ?  1 : -1;
        dy    = y2 - y1;
        dx    = abs( x2-x1 );
        d     = 2 * dx - dy;
        aincr = 2 * (dx - dy);
        bincr = 2 * dx;
        x     = x1;
        y     = y1;
        Pixel( x, y, colour );
        for( y=y1+1; y<= y2; ++y )
        {
            if ( d >= 0 )
            {
                x += xincr;
                d += aincr;
            }
            else
            d += bincr;
            Pixel( x, y, colour );
        }
    }
    else
    {
        if( x1 > x2 )
        {
            XCHG( x1, x2, long );
            XCHG( y1, y2, long );
        }
        yincr = ( y2 > y1 ) ? 1 : -1;
        dx    = x2 - x1;
        dy    = abs( y2-y1 );
        d     = 2 * dy - dx;
        aincr = 2 * (dy - dx);
        bincr = 2 * dy;
        x     = x1;
        y     = y1;
        Pixel( x, y, colour );
        for(x=x1+1; x<=x2; ++x )
        {
            if( d >= 0 )
            {
                y += yincr;
                d += aincr;
            }
            else
            d += bincr;
            Pixel( x, y, colour );
        }
    }
    return;
}


void static Demo2(void)
{
    int y = 0,pasy = 1;
    int x = 0,pasx = 1;
    do
    {
      lpBackBuffer= VBE_LockSurface();
      // slow clearing
      memset(lpBackBuffer,0,VBE_mode.BytesPerLine*VBE_mode.YResolution);
      // Draw 2 moving lines
      y+=pasy;
      x+=pasx;
      B_Line(0,y,VBE_mode.XResolution-1,VBE_mode.YResolution-y,y);
      B_Line(x,0,VBE_mode.XResolution-x,VBE_mode.YResolution-1,0xffffffff-x);

      if ((y>=VBE_mode.YResolution)||(y<=0)) pasy=-pasy;
      if ((x>=VBE_mode.XResolution)||(x<=0)) pasx=-pasx;

      // Flip page 1 and 2
      VBE_UnlockSurface(); // it's a good habit to unlockSurface before flipping.
      VBE_Flip();
      // Loop until a key
    }while(!kbhit());
    return;
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
            if (VBE_GetCaps() & VESACAPS_LINEAR)
            {
                Demo2();
                VGA_SetDisplayMode(0x3);
            }
            else
            {
                VGA_SetDisplayMode(0x3);
                printf("couldn't linearize the video memory.");
            }

        }
        else
        {
            VGA_SetDisplayMode(0x3);
            printf("couldn't initialize mode %x (error code=%x)\n",mode, hr);
            getch();
        }
        VBE_Release();
        RM_Release();
    }
    else
    {
        printf("couldn't set up VBE \n");
    }
    return 0;
}
