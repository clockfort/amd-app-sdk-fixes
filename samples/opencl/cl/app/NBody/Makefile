DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= NBody
EXE_TARGET_INSTALL   	= NBody

####
#
#  C/CPP files
#
####

FILES 	= NBody
CLFILES = NBody_Kernels.cl 

LLIBS  += SDKUtil

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

