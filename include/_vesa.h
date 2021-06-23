/****************************************************************************
; *                                                                         *
; *     File        : _vesa.h
; *                                                                         *
; *     Description : VESA Services
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
#ifndef __VESA_HH
#define __VESA_HH

#include "_std.h"

typedef int VBE_MODE;
typedef int VBE_RESULT;
typedef int VBEPM_STATE;

#define RGBMask(r,g,b) \
( (  (r)>>(8-VBE_mode.RedMaskSize  )) << VBE_mode.RedFieldPosition   )\
+ ( ((g)>>(8-VBE_mode.GreenMaskSize)) << VBE_mode.GreenFieldPosition )\
+ ( ((b)>>(8-VBE_mode.BlueMaskSize )) << VBE_mode.BlueFieldPosition  )

enum VESACAPS { 
     VESACAPS_LINEAR       = 0x1,
     VESACAPS_STEREODEVICE = 0x2, // VBE 2.1 specifications
     VESACAPS_PAGECOUNT    = 0x4,
     VESACAPS_FORCEDWORD   = 0xffff
};

enum VBEPM_STATE {
      VBEPM_ON,
      VBEPM_STANDBY,
      VBEPM_SUSPEND,
      VBEPM_OFF,
      VBEPM_REDUCEDON,
      VBEPM_FORCEDWORD = 0xffff
};

enum VBE_DACMODE {
      VBE_PALNA,
      VBE_PAL6BITDAC,
      VBE_PAL8BITDAC,
      VBE_PALFORCEDWORD = 0xffff
};

enum VESA_MODE_ATTR {
      VESA_MODE_ATTR_HARDWARE       =0x00000001, /* Hardware supported */
      VESA_MODE_ATTR_TTY            =0x00000004, /* Has BIOS TTY funcs */
      VESA_MODE_ATTR_COLOR          =0x00000008, /* Is a color mode */
      VESA_MODE_ATTR_GRAPHICS       =0x00000010, /* Graphics or Text? */
      VESA_MODE_ATTR_VGA_COMPAT     =0x00000020, /* Is VGA compatable? */
      VESA_MODE_ATTR_MEMORY_VGA     =0x00000040, /* VGA windowed memory? */
      VESA_MODE_ATTR_MEMORY_LINEAR  =0x00000080, /* Linear memory? */
      VESA_MODE_ATTR_DOUBLE_SCAN    =0x00000100, /* Double scan mode? */
      VESA_MODE_ATTR_INTERLACED     =0x00000200, /* Interlaced? */
      VESA_MODE_ATTR_TRIPLE_BUFFER  =0x00000400, /* Triple-buffered ? */
      VESA_MODE_ATTR_STEREO         =0x00000800, /* Stereo hardware? */
      VESA_MODE_ATTR_DUAL_DISPLAY   =0x00001000  /* Dual Display? */
};

// Error API codes
#define VBEERR_NOERR                 0x0000
#define VBEERR_SETDISPLAYMODEFAILED  0x1000
#define VBEERR_ACCESSRIGHTFAILED     0x1001
#define VBEERR_PHYSICALADDRMAPFAILED 0x1002
#define VBEERR_CANTREADMODEINFO      0x1003
#define VBEERR_NOVESADETECTED        0x1004
#define VBEERR_ALLOCLDTFAILED        0x1005
#define VBEERR_NONLINEAR             0x1006
#define VBEERR_UNKNOWN               0xffff

typedef struct _vbe_CoreInfoBlock {

    /* VBE 1.0 and above */
    char            VESASignature[4];    // 4 signature bytes
    short int       VESAVersion;         // VESA version number
    const char*     OEMStringPtr;        // Pointer to OEM string (name of the VESA Bios)
    unsigned char   Capabilities[4];     // capabilities of the video environment
    const ushort*   VideoModePtr;        // pointer to supported Super VGA modes.
    short int       memory;              // Number of 64kb memory blocks on board. Use VBE_GetTotalVidMem() to get this value in bytes.

    /* VBE 2.0 and above */
    short           OEMSoftwareRev;
    const char*     OEMVendorNamePtr;
    const char*     OEMProductNamePtr;   
    const char*     OEMProductRevPtr;
    const char      reserved[222];
    const char      OemDATA [256];
}VBE_CoreInfoBlock;

typedef struct _vbe_ModeInfoBlock{

    /* VBE 1.0 and above */
    short int       ModeAttributes;      // See VBE_MODEATTR
    unsigned char   WinAAttributes;
    unsigned char   WinBAttributes;
    short int       WinGranularity;
    short int       WinSize;
    short int       WinASegment;
    short int       WinBSegment;
    long int        WinFuncPtr;
    short int       BytesPerLine;
    short int       XResolution;         // horizontal resolution
    short int       YResolution;         // vertical resolution
    unsigned char   XCharSize;           // character cell width
    unsigned char   YCharSize ;          // character cell height
    unsigned char   NumberOfPlanes;      // number of memory planes
    unsigned char   BitsPerPixel;        // bits per pixel
    unsigned char   NumberOfBanks;       // number of banks
    unsigned char   MemoryModel;         // memory model type
    unsigned char   BankSize;            // bank size in kb
    unsigned char   NumberOfImagePages;  // number of images
    unsigned char   Res1;                // reserved for page function
    unsigned char   RedMaskSize;         // size of direct color Red mask in bits
    unsigned char   RedFieldPosition;    // bit position of LSB of Red mask
    unsigned char   GreenMaskSize;       // size of direct color green mask in bits
    unsigned char   GreenFieldPosition ; // bit position of LSB of green mask
    unsigned char   BlueMaskSize       ; // size of direct color blue mask in bits
    unsigned char   BlueFieldPosition  ; // bit position of LSB of blue mask
    unsigned char   RsvdMaskSize       ; // size of direct color reserved mask in bits
    unsigned char   RsvdFieldPosition  ; // size of direct color reserved mask in bits
    unsigned char   DirectColorModeInfo; // Direct Color mode attributes

    /* VBE 2.0 and above */
    long            PhysicBasePtr;       // Physical Address for linear buffer
    long            OffScreenMemOffset;
    short           OffScreenMemSize;

    /* VBE 3.0 and above ( Source: Jason McMullan ) */
    unsigned char   linear_scan_line_size;
    unsigned char   banked_image_pages;
    unsigned char   linear_image_pages;
    unsigned char   linear_RedMaskSize;
    unsigned char   linear_RedFieldPosition;
    unsigned char   linear_GreenMaskSize;
    unsigned char   linear_GreenFieldPosition;
    unsigned char   linear_BlueMaskSize;
    unsigned char   linear_BlueFieldPosition;
    unsigned char   linear_RsvdMaskSize;
    unsigned char   linear_RsvdFieldPosition;
    unsigned char   max_pixel_clock;    /* Max pixel clock (in Hz) */
    char            reserved2[189];
}VBE_ModeInfoBlock;


typedef struct _vbepm_info {
    int minor        : 4;
    int major        : 4;
    int hasStandy    : 1;
    int hasSuspend   : 1;
    int hasoff       : 1;
    int hasReduceOn  : 1;
}VBEPM_Info;

#define VBE_GetTotalVidMem() ((ulong)VBE_Core.memory * 65536)

typedef (*VBE_ENUMDISPLAYCALLBACK) (const VBE_ModeInfoBlock *info, VBE_MODE mode, void *context);

#ifdef __cplusplus
extern "C" {
#endif
    // VBE Services
    VBE_RESULT          VBE_Initialize(); // Initialize VBE engine
    void                VBE_Release(); // Release VBE engine
    VBE_MODE            VBE_FindDisplayMode(int width, int height,int bitsPerPixel);  // Find a desiRed display mode according criteria (width, height and screen depth)
    VBE_RESULT          VBE_SetDisplayMode(VBE_MODE mode); // Set desired display mode
    int                 VBE_EnumDisplayModes(VBE_ENUMDISPLAYCALLBACK callback, void *context); // Enumerate all display modes. Each valid, video modes a call to <callback> with a custom parameter is called.
    const char *        VBE_GetStringErr(VBE_RESULT err); // Error string conversion
    // VBE 8bit mode Services
    VBE_RESULT          VBE_SetDac(int mode); // Set VGA 8bit DAC precision (6bit or 8bit)
    VBE_RESULT          VBE_GetPalette(int start, int numEntries, void *pal); // Get current 8bit palette. Only in 8bit display modes. start : first color entry (0), numEntries : number of colros (256), pal (buffer of <numEntries> RGB values (3 x unsigned char)
    void CALLING_C      VBE_SetPalette(ulong start, ulong numEntries, const void * pal); // Set 8bit palette. Only in 8bit display modes. 

    // CRTC Services and frame buffer access.
    void    CALLING_C   VBE_Flip(void); // Flip backbuffer/front buffer.
    void                VBE_SetBank(int bank); // Change video bank (just given if you want to manage VESA 1.2).
    uchar*              VBE_LockSurface(void); // Return the current frame buffer video address (where you can read/write)
    void                VBE_LockSurfaceAlt(uchar **front, uchar **back, long *pitch); // Same as lockSurface but give front buffer, back buffer and pitch (bytes per lines).
    void                VBE_UnlockSurface(void); // Not really required to unlock the surface (invalid current display video pointer).
    VBE_RESULT          VBE_SetScanlineLength(long new_length); // Set the scanline length. Useful for horizontal scrolling .
    VBE_RESULT          VBE_SetDisplayOffset(long x,long y); // Set scrolling position. Use full for horizontal/vertical scrolling.
    
    // Power Management Services
    VBE_RESULT          VBEPM_Initialize(void); // Some functions for Power Management functions.
    void                VBEPM_SetState(VBEPM_STATE mode); // set a VBEPM_STATE mode
    VBEPM_STATE         VBEPM_GetState(void); // Return a VBEPM_STATE
    ulong               VBE_GetCaps();        // Get raster caps

    // VGA Services
    VBE_RESULT          VGA_SetDisplayMode(int mode); // Set a VGA display mode (ie 0x3 is 80x25 mode, 0x13 is 320x200x8bit)
    void                VGA_WaitVBL(); // Wait vertical blank.


    // Global variables
    extern VBE_CoreInfoBlock VBE_Core; // You can retrieve here, VESA driver informations.
    extern VBE_ModeInfoBlock VBE_mode; // You can retrieve here, screen resolution and more information
    

#ifdef __cplusplus
}

// C++ Interface

class VBECore  {
    public:
        VBECore() { _ok = VBE_Initialize();}
        ~VBECore() { VBE_Release();}
        int InitCheck() { return _ok;}
        VBE_MODE FindDisplayMode(int width, int height, int bitsPerPixel) { return VBE_FindDisplayMode(width, height, bitsPerPixel);}
        int EnumDisplayModes(VBE_ENUMDISPLAYCALLBACK callback, void *context) {return VBE_EnumDisplayModes(callback, context);}
        VBE_RESULT SetDisplayMode(VBE_MODE mode) { return VBE_SetDisplayMode(mode);}
        void Flip() {VBE_Flip();}
        uchar *LockSurface() {return VBE_LockSurface();}
        void UnlockSurface() {VBE_UnlockSurface();}
        VBE_RESULT SetDisplayOffset(long x, long y) { return VBE_SetDisplayOffset(x,y); }
        VBE_RESULT SetScanlineLength(int l) { return VBE_SetScanlineLength(l); }
        ulong GetCaps() { return VBE_GetCaps();}
    
    private:
        int _ok;
}
#endif


#endif // __VESA_HH

