DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= SimpleImage
EXE_TARGET_INSTALL   	= SimpleImage

####
#
#  C/CPP files
#
####

FILES 	= SimpleImage
CLFILES	= SimpleImage_Kernels.cl
IMAGES	= SimpleImage_Input.bmp

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

