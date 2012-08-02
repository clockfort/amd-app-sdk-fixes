DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= QuasiRandomSequence
EXE_TARGET_INSTALL   	= QuasiRandomSequence

####
#
#  C/CPP files
#
####

FILES 	= QuasiRandomSequence SobolPrimitives
CLFILES	= QuasiRandomSequence_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

