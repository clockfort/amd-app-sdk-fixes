DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= BoxFilter
EXE_TARGET_INSTALL   	= BoxFilter

####
#
#  C/CPP files
#
####

FILES 	= BoxFilter BoxFilterSeparable BoxFilterSAT
CLFILES	= BoxFilter_Kernels.cl 
IMAGES	= BoxFilter_Input.bmp

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

