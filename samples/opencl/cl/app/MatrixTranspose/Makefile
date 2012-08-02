DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= MatrixTranspose
EXE_TARGET_INSTALL   	= MatrixTranspose

####
#
#  C/CPP files
#
####

FILES 	= MatrixTranspose
CLFILES	= MatrixTranspose_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

