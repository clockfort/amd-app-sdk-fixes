DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= URNG
EXE_TARGET_INSTALL   	= URNG

####
#
#  C/CPP files
#
####

FILES 	= URNG
CLFILES	= URNG_Kernels.cl
IMAGES	= URNG_Input.bmp

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

