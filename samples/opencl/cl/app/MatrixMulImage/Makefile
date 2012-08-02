DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= MatrixMulImage
EXE_TARGET_INSTALL   	= MatrixMulImage

####
#
#  C/CPP files
#
####

FILES 	= MatrixMulImage
CLFILES	= MatrixMulImage_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

