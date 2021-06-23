#/************************************************************************
# *
# *     File        : MAKEFILE.
# *
# *     Description : Makefile for VESA 2.00 TEST
# *
# *     Copyright (C) 1995,97 RealTech
# *
#************************************************************************/


.EXTENSIONS:
.EXTENSIONS: .exe .obj .cpp .c .asm
.BEFORE
	@set INCLUDE=.;$(%watcom)\h;$(%watcom)\h\win;..\include

CC            = wcc386.exe
wcc_options   = /mf /d0 /wx /Oneatx /5 /fp5

.c.obj:
	$(CC) $[* $(wcc_options)

ALL : ..\demo0.exe ..\demo1.exe ..\demo2.exe
..\demo0.exe: demo0.obj ..\lib\vbelib.lib
        wlink @demo0.lnk

..\demo1.exe: demo1.obj ..\lib\vbelib.lib
        wlink @demo1.lnk

..\demo2.exe: demo2.obj ..\lib\vbelib.lib
        wlink @demo2.lnk


