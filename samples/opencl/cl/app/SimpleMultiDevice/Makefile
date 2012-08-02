DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= SimpleMultiDevice
EXE_TARGET_INSTALL   	= SimpleMultiDevice

####
#
#  C/CPP files
#
####

FILES 	= SimpleMultiDevice
CLFILES	= SimpleMultiDevice_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

