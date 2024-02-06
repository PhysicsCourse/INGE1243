.AUTODEPEND

#		*Translator Definitions*
CC = tcc +MGR.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\TC\LIB
INCLUDEPATH = C:\TC\INCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 svga\svga256f.obj \
 svgautil.obj \
 mgr.obj \
 mgr2.obj \
 gifsave.obj \
 mgrlib\egavga.obj \
 mgrlib\litt.obj \
 mgrlib\trip.obj

#		*Explicit Rules*
mgr.exe: mgr.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0h.obj+
svga\svga256f.obj+
svgautil.obj+
mgr.obj+
mgr2.obj+
gifsave.obj+
mgrlib\egavga.obj+
mgrlib\litt.obj+
mgrlib\trip.obj
mgr
		# no map file
graphics.lib+
fp87.lib+
mathh.lib+
ch.lib
|


#		*Individual File Dependencies*
svgautil.obj: mgr.cfg svgautil.c 

mgr.obj: mgr.cfg mgr.c 

mgr2.obj: mgr.cfg mgr2.c 

gifsave.obj: mgr.cfg gifsave.c 

#		*Compiler Configuration File*
mgr.cfg: mgr.mak
  copy &&|
-mh
-2
-f287
-K
-G
-O
-Z
-r-
-d
-h
-b-
-vi-
-w-ret
-w-nci
-w-inl
-w-par
-w-cpt
-w-dup
-w-pia
-w-ill
-w-sus
-w-ext
-w-ias
-w-ibc
-w-pre
-w-nst
-I$(INCLUDEPATH)
-L$(LIBPATH)
-Ff
| mgr.cfg


