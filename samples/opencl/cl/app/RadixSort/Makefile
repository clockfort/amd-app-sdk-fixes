DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= RadixSort
EXE_TARGET_INSTALL   	= RadixSort

####
#
#  C/CPP files
#
####

FILES 	= RadixSort
CLFILES	= RadixSort_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

