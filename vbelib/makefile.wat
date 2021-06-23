#;/************************************************************************
#;*
#;*     File        :   MAKEFILE.WAT
#;*
#;*     Description :   Makefile for VESA 2.00 Lib (Watcom 10 or later)
#;*
#;*     Copyright (C) 1994,97 Realtech
#;*
#;***********************************************************************/

.EXTENSIONS:
.EXTENSIONS: .exe .obj .asm .c .cpp
.BEFORE

        @SET INCLUDE=.;$(%watcom)\h;..\include;

CC   = wcc386 -Otexan -5 -WX -d0 
LIB  = wlib
LINK = wlink

.c.obj:
  $(CC) $?

.cpp.obj:
  $(CC) $?


OBJECTS = wdpmi.obj vbecore.obj dpmi.obj 

..\lib\vbelib.lib: $(OBJECTS)
  $(LIB) -c -n -b $@ $(OBJECTS)
