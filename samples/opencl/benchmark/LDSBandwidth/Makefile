DEPTH = ../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= LDSBandwidth
EXE_TARGET_INSTALL   	= LDSBandwidth

####
#
#  C/CPP files
#
####

FILES 	= LDSBandwidth
CLFILES	= LDSBandwidth_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

