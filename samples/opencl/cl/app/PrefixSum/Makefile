DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= PrefixSum
EXE_TARGET_INSTALL   	= PrefixSum

####
#
#  C/CPP files
#
####

FILES 	= PrefixSum
CLFILES	= PrefixSum_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

