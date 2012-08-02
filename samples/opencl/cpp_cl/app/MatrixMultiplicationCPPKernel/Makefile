DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= MatrixMultiplicationCPPKernel
EXE_TARGET_INSTALL   	= MatrixMultiplicationCPPKernel

####
#
#  C/CPP files
#
####

FILES 	= MatrixMultiplicationCPPKernel
CLFILES	= MatrixMultiplicationCPPKernel_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

