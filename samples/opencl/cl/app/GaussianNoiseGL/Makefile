DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= GaussianNoiseGL
EXE_TARGET_INSTALL   	= GaussianNoiseGL

####
#
#  C/CPP files
#
####

FILES 	= GaussianNoiseGL 
CLFILES = GaussianNoiseGL_Kernels.cl GaussianNoiseGL_Kernels2.cl
IMAGES	= GaussianNoiseGL_Input.bmp

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

include $(DEPTH)/make/openclsdkrules.mk 



