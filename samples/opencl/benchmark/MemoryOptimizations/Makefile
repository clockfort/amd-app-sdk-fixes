DEPTH = ../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= MemoryOptimizations
EXE_TARGET_INSTALL   	= MemoryOptimizations

####
#
#  C/CPP files
#
####

FILES 	= MemoryOptimizations
CLFILES	= MemoryOptimizations_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

