DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= MatrixMultiplication
EXE_TARGET_INSTALL   	= MatrixMultiplication

####
#
#  C/CPP files
#
####

FILES 	= MatrixMultiplication
CLFILES	= MatrixMultiplication_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

