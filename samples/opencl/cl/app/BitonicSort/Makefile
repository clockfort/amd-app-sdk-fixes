DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= BitonicSort
EXE_TARGET_INSTALL   	= BitonicSort

####
#
#  C/CPP files
#
####

FILES 	= BitonicSort
CLFILES	= BitonicSort_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

