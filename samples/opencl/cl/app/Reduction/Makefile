DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= Reduction
EXE_TARGET_INSTALL   	= Reduction

####
#
#  C/CPP files
#
####

FILES 	= Reduction
CLFILES	= Reduction_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

