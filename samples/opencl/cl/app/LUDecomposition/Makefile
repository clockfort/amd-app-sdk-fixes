DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= LUDecomposition
EXE_TARGET_INSTALL   	= LUDecomposition

####
#
#  C/CPP files
#
####

FILES 	= LUDecomposition
CLFILES	= LUDecomposition_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

