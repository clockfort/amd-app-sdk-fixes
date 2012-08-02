DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= MersenneTwister
EXE_TARGET_INSTALL   	= MersenneTwister

####
#
#  C/CPP files
#
####

FILES 	= MersenneTwister
CLFILES	= MersenneTwister_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

