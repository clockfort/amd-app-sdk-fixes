DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= DwtHaar1DCPPKernel
EXE_TARGET_INSTALL   	= DwtHaar1DCPPKernel

####
#
#  C/CPP files
#
####

FILES 	= DwtHaar1DCPPKernel
CLFILES	= DwtHaar1DCPPKernel_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

