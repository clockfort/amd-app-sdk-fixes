DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= DwtHaar1D
EXE_TARGET_INSTALL   	= DwtHaar1D

####
#
#  C/CPP files
#
####

FILES 	= DwtHaar1D
CLFILES	= DwtHaar1D_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

