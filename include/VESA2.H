;/************************************************************************
; *
; *     File        : VESA.H
; *
; *     Description : Vesa header for VESA 2.00
; *
; *     Copyright (C) 1995 RealTech
; *
; ************************************************************************/
#ifndef _SUPERVGA
#define _SUPERVGA
#pragma pack(1)
typedef struct {
    short int  ModeAttributes;
    unsigned char WinAAttributes;
    unsigned char WinBAttributes;
    short int  WinGranularity;
    short int  WinSize;
    short int  WinASegment;
    short int  WinBSegment;
    long int WinFuncPtr;
    short int BytesPerLine;
    short int XResolution;             // horizontal resolution
    short int YResolution ;            // vertical resolution
    unsigned char XCharSize;           // character cell width
    unsigned char YCharSize ;          // character cell height
    unsigned char NumberOfPlanes;      // number of memory planes
    unsigned char BitsPerPixel ;       // bits per pixel
    unsigned char NumberOfBanks;       // number of banks
    unsigned char MemoryModel  ;       // memory model type
    unsigned char BankSize      ;      // bank size in kb
    unsigned char NumberOfImagePages;  // number of images
    unsigned char Res1              ;  // reserved for page function
    unsigned char RedMaskSize   ;      // size of direct color red mask in bits
    unsigned char RedFieldPosition ;   // bit position of LSB of red mask
    unsigned char GreenMaskSize    ;   // size of direct color green mask in bits
    unsigned char GreenFieldPosition ; // bit position of LSB of green mask
    unsigned char BlueMaskSize       ; // size of direct color blue mask in bits
    unsigned char BlueFieldPosition  ; // bit position of LSB of blue mask
    unsigned char RsvdMaskSize       ; // size of direct color reserved mask in bits
    unsigned char RsvdFieldPosition  ; // size of direct color reserved mask in bits
    unsigned char DirectColorModeInfo; // Direct Color mode attributes
    long PhysBasePtr;                  // Physical Address for linear buffer
    long OffScreenMemOffset;
    short OffScreenMemSize;
    char res2[206];
}VesaModeInfo;
typedef struct {
    char VESASignature[4];             // 4 signature bytes
    short int VESAVersion;             // VESA version number
    char *OEMStringPtr;                // Pointer to OEM string
    unsigned char Capabilities[4];     // capabilities of the video environment
    void *VideoModePtr;                // pointer to supported Super VGA modes
    short int memory;                  // Number of 64kb memory blocks on board
    short OemSoftwareRev;
    long OemVendorNamePtr;
    long OemProductNamePtr;
    long OemProductRevPtr;
    char reserved[222];
    char OemDATA [256];
}VgaInfoBlock;
#define pragma pack()
#ifdef __cplusplus
extern "C" {
    #endif
    /* ************** C *******************
    */
    int Init_Vesamode(int VESAmode);
    void screen_vesa640x480(void);
    void VESAsetScanLineLength(long dx);
    void VESAsetXY(long x,long y);
    unsigned char set8BitPalette(void);
    unsigned char set6BitPalette(void);
    void cdecl setVESAPalette(unsigned long a, unsigned long b, void *pal);
    void getVESAPalette(int start, int num, unsigned char *palbuf);
    int Get_VesaInfo(int VESAmode);
    /* *********** Variables **************
    */
    extern VgaInfoBlock vesabuf;
    extern VesaModeInfo modebuf;
    extern short int VESAbankshift,VideoCardCode,VESA_selector,VOffPage;
    extern char far *LFBPtr;
    extern unsigned char SVGA_widedac,useLinearVESA,Current_VRAM_Page;
    #ifdef __cplusplus
}
#endif
#define vbeMemPK       4
#define vbeUseLFB      0x4000
#define vbeMdAvailable 0x0001
#define vbeMdColorMode 0x0008
#define vbeMdGraphMode 0x0010
#define vbeMdNonbanked 0x0040
#define vbeMdLinear    0x0080
#define LfbMapRealPointer(p) (void*)(((unsigned)(p)>>12)+((p) & 0xFFFF))
#define LfbGetTicks    *((long*)0x46C)
enum {VESA_320x200=0x153,VESA_360x200 =0x156,VESA_320x240  =0x154,
      VESA_360x240=0x157,VESA_320x400 =0x155,VESA_360x400  =0x158,
      VESA_640x350=0x11C,VESA_640x400 =0x100,VESA_640x480  =0x101,
      VESA_800x600=0x101,VESA_1024x768=0x105,VESA_1280x1024=107};

#endif
