





   VESA 3.00 Programming Developpement Kit v3.0
   Coding by Execom of Realtech  (execom@www.scene.org)
   http://www.realtech.scene.org











a) Introduction 

     For more informations about VESA 2.0 go to http://www.vesa.org

     VESA 2.00 enable you to work directly in video under DOS without any slowing banks switching (due to 16bit compatibility)

     It's running on most DOS extender (DOS4/GW 1.95 or later, earlier version won't recognize some DPMI functions and 32 bit compiler like Watcom 10.6 or 11, and DJGPP v2.). It wasn't designed for 16bit compiler.

     This library has been tested on most of configurations :

      - Matrox Millenium.
      - Chips 65550.      
      - S3 Virge.
      - S3 Trio 64/64V+.
      - S3 968.
      - ET4000/W32p.
      - ATI Rage Pro
      - ATI Rage 128
      - NVidia GeForce

     And many others. It's compatible with Windows 9x (not NT) 

     On some videos card, under Windows, the display driver failed to restore the correct monitor
     frequencies, so you may returns into Windows with black screen or scrambled. It's due to a bug
     on theses drivers. In that case, please runs exclusivly under DOS.

     On Matrox videos card, "15bit modes are 16bits(1)" and "800x600 is 960 pixels columns(2)"
     bugs are automatically fixed by the API.

     (1) When you ask a 15bit mode, it returns a 16bit modes
     (2) When setting 800x600 mode, the scanline length is 960 instead of 800

     A very safe LFB allocating AND deallocating for stability and memory guard prevents.
   
     There is also a C++ wrapper for C++ programmers.

     A separated DPMI Service interface, DJGPP compatible, enables you to simulate real mode interrupts under Dos/4g or DJGPP easily (like CDROM/Mscdex), and additional services are also given(memory available, CPU informations etc...)

     This is a compatibility with VESA 1.2. There is some functions that can help you to change banks (I suggest then to work in RAM, then copying 64Kb by 64Kb and changing bank each time).

    About VBE 2.1 (or VESA 3.00 extensions), there is some features that are not really taken advantage
here (due some buggy implementations on some video cards).

b) Compiling

  
   Warning : The archive contains subdirectories.

   If you have Watcom 10 or later, rename makefile.wat as makefile and compile.
   If you have Djgpp v2 or later, rename makefile.gcc as makefile and compile.

   /VBELIB : contains VBE Core Library sources files and RM DPMI manager
   /DEMOS : contains demos
   /LIB : VBE Core lib
   /INCLUDE : includes files

   There is 3 demos

   demo0.exe : VBE detection and enumerate all display modes. Notes that you can have several times the
   same video modes on some video card (if you generate a list, you must check the 'unicity' of the       display mode).

   demo1.exe : LFB writing test. The program set into 640x480x16 bits full screen, linear frame buffer
   and draw a Julia Fractal. Also use VBE_LockSurfaceAlt to get the front buffer.

   demo2.exe : Page Flipping test and back buffer writing. The program draw some lines using page flip mode.


c) Copyright

    This SDK is free, but you mustn't change anything in this package! Some Credits is welcome if you're     using in your program. This SDK was cloned in some version, but heh, it doesn't matter, this SDK will be the first for VESA 2.0.
    You can mail for more information at realtech@www.scene.org about it.

* END OF FILE
