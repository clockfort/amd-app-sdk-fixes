DEPTH = ../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= GlobalMemoryBandwidth
EXE_TARGET_INSTALL   	= GlobalMemoryBandwidth

####
#
#  C/CPP files
#
####

FILES 	= GlobalMemoryBandwidth
CLFILES	= GlobalMemoryBandwidth_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

