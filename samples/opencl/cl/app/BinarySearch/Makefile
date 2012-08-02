DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= BinarySearch
EXE_TARGET_INSTALL   	= BinarySearch

####
#
#  C/CPP files
#
####

FILES 	= BinarySearch
CLFILES	= BinarySearch_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

