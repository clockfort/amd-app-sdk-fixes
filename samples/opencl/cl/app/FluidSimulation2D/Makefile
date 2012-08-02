DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= FluidSimulation2D
EXE_TARGET_INSTALL   	= FluidSimulation2D

####
#
#  C/CPP files
#
####

FILES 	= FluidSimulation2D ColorScale
CLFILES = FluidSimulation2D_Kernels.cl 

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

