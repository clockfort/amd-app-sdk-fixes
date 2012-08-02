DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= Mandelbrot
EXE_TARGET_INSTALL   	= Mandelbrot

####
#
#  C/CPP files
#
####

FILES 	= Mandelbrot MandelbrotDisplay
CLFILES	= Mandelbrot_Kernels.cl

LLIBS  	+= SDKUtil

ifeq ($(OS),lnx)
		ifdef release
			C_RELEASE_FLAG := $(subst -ffast-math,, $(C_RELEASE_FLAG))
		endif
endif

ifeq ($(OS),lnx)
ifdef MINGW
	LLIBS 	+= glew$(BITS).dll glut$(BITS) opengl32 glu32
else
	LLIBS 	+= glut GLEW GL GLU
endif
else
	LLIBS += glut$(BITS) GLEW$(BITS)
endif


include $(DEPTH)/make/openclsdkrules.mk 

