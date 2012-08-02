DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= MatrixMulDouble
EXE_TARGET_INSTALL   	= MatrixMulDouble

####
#
#  C/CPP files
#
####

FILES 	= MatrixMulDouble
CLFILES	= MatrixMulDouble_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

