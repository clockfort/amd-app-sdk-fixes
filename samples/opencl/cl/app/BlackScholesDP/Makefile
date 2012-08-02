DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= BlackScholesDP
EXE_TARGET_INSTALL   	= BlackScholesDP

####
#
#  C/CPP files
#
####

FILES 	= BlackScholesDP
CLFILES	= BlackScholesDP_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

