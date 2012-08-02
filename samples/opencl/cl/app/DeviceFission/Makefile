DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= DeviceFission
EXE_TARGET_INSTALL   	= DeviceFission

####
#
#  C/CPP files
#
####

FILES 	= DeviceFission
CLFILES	= DeviceFission_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

