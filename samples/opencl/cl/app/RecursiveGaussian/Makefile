DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= RecursiveGaussian
EXE_TARGET_INSTALL   	= RecursiveGaussian

####
#
#  C/CPP files
#
####

FILES 	= RecursiveGaussian
CLFILES	= RecursiveGaussian_Kernels.cl
IMAGES	= RecursiveGaussian_Input.bmp

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

