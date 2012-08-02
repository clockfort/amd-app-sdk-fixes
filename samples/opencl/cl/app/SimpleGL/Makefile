DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= SimpleGL
EXE_TARGET_INSTALL   	= SimpleGL

####
#
#  C/CPP files
#
####

FILES 	= SimpleGL
CLFILES = SimpleGL_Kernels.cl
IMAGES	= SimpleGL.bmp

LLIBS  += SDKUtil

ifeq ($(OS),lnx)
ifdef MINGW
	LLIBS += glew$(BITS).dll glut$(BITS) opengl32 glu32 gdi32
else
	LLIBS += glut GLEW GL GLU
endif
else
	LLIBS += glut$(BITS) GLEW$(BITS)
	LLIBS  += kernel32 gdi32 user32 advapi32 Shlwapi
endif

INCLUDEDIRS += $(SDK_HEADERS) 


include $(DEPTH)/make/openclsdkrules.mk 

