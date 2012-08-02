DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= BlackScholes
EXE_TARGET_INSTALL   	= BlackScholes

####
#
#  C/CPP files
#
####

FILES 	= BlackScholes
CLFILES	= BlackScholes_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

