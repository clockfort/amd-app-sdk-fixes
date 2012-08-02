DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= SobelFilter
EXE_TARGET_INSTALL   	= SobelFilter

####
#
#  C/CPP files
#
####

FILES 	= SobelFilter
CLFILES	= SobelFilter_Kernels.cl
IMAGES	= SobelFilter_Input.bmp

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

