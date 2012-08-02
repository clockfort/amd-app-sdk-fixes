DEPTH = ../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= ConstantBandwidth
EXE_TARGET_INSTALL   	= ConstantBandwidth

####
#
#  C/CPP files
#
####

FILES 	= ConstantBandwidth
CLFILES	= ConstantBandwidth_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

