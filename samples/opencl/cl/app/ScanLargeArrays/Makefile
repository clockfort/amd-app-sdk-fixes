DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= ScanLargeArrays
EXE_TARGET_INSTALL   	= ScanLargeArrays

####
#
#  C/CPP files
#
####

FILES 	= ScanLargeArrays
CLFILES	= ScanLargeArrays_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

