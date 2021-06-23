/****************************************************************************
; *                                                                         *
; *     File        : demo0.c
; *                                                                         *
; *     Description : VESA 2.0 - Enumerate display modes
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

#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "_std.h"
#include "_wdpmi.h"
#include "_dpmi.h" // Required for RM_initialize interface
#include "_vesa.h"

void static DisplayInformations()
{
    printf("VBE Version      : %d.%d\n",VBE_Core.VESAVersion>>8,VBE_Core.VESAVersion&0xff);
    printf("OEM String       : %s\n",VBE_Core.OEMStringPtr);    
    printf("OEM Product name : %s\n",VBE_Core.OEMProductNamePtr);
    printf("OEM Vendor name  : %s\n",VBE_Core.OEMVendorNamePtr);
    printf("OEM Product rev  : %s\n",VBE_Core.OEMProductRevPtr);
    printf("press any key ...\n");
    getch();
    return;
}

int callback(const VBE_ModeInfoBlock *info, VBE_MODE mode, void *context)
{
    int *v = (int*)context;

    char tex[256];
    *tex = 0;
    if ((info->BitsPerPixel>=15)&&(info->BitsPerPixel<=16))
    {
        sprintf(tex,"[%d:%d:%d]",info->RedMaskSize,info->GreenMaskSize, info->BlueMaskSize);
    }
    if (info->ModeAttributes&VESA_MODE_ATTR_HARDWARE) strcat(tex," Hardware.");
    if (info->ModeAttributes&VESA_MODE_ATTR_TTY) strcat(tex," TTY funcs.");
    if (info->ModeAttributes&VESA_MODE_ATTR_COLOR) strcat(tex," Color.");
    if (info->ModeAttributes&VESA_MODE_ATTR_GRAPHICS) strcat(tex," Graphics."); else strcat(tex, " text");
    if (info->ModeAttributes&VESA_MODE_ATTR_VGA_COMPAT) strcat(tex," VGA"); else strcat(tex, " non-VGA");
    if (info->ModeAttributes&VESA_MODE_ATTR_MEMORY_LINEAR) strcat(tex," Linear");
    if (info->ModeAttributes&VESA_MODE_ATTR_DOUBLE_SCAN) strcat(tex," DbScan");
    if (info->ModeAttributes&VESA_MODE_ATTR_INTERLACED) strcat(tex," Interlaced");
    if (info->ModeAttributes&VESA_MODE_ATTR_TRIPLE_BUFFER) strcat(tex," 3buf");
    if (info->ModeAttributes&VESA_MODE_ATTR_STEREO) strcat(tex," ST");
    if (info->ModeAttributes&VESA_MODE_ATTR_DUAL_DISPLAY) strcat(tex," Dual");
    
    printf("(%02d) %04xh : %4dx%4d %2d bits (%s)\n",*v, mode, info->XResolution, info->YResolution, info->BitsPerPixel, tex);
    (*v)++;
    if (((*v)%20)==0)
    {
        printf("press any key ...\n");
        getch();
    }
    return 1;
}

// ======================================================== main program
main()
{
    int hr;    
    printf("\n\nRealtech VBE Kit v3.0 (C) Copyright Realtech 1997,2000\nhttp://www.realtech.scene.org\n\n");
    
    RM_Initialize(); // First initialize the RM manager.
    hr = VBE_Initialize(); // Then initialize the VBE Core engine.
    if (hr == VBEERR_NOERR)
    {
        int v = 0;
        DisplayInformations(); // Display driver informations 
        printf("\nList all available video modes :\n"); // Enumerate test 
        VBE_EnumDisplayModes(callback, &v);
        VBE_Release(); // Freeing the VBE Core engine        
    }
    else
    {
        printf("Couldn't set up VBE (%s)\n",VBE_GetStringErr(hr));
    }
    RM_Release(); // Freeing the RM manager.
    return 0;
}
