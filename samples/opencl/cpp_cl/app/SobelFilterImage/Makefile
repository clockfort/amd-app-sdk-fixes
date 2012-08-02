DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= SobelFilterImage
EXE_TARGET_INSTALL   	= SobelFilterImage
####
#
#  C/CPP files
#
####

FILES 	= SobelFilterImage
CLFILES = SobelFilterImage_Kernels.cl 
IMAGES = SobelFilterImage_Input.bmp

LLIBS  	+= SDKUtil


include $(DEPTH)/make/openclsdkrules.mk 

