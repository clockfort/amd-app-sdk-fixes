DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= FFT
EXE_TARGET_INSTALL   	= FFT

####
#
#  C/CPP files
#
####

FILES 	= FFT
CLFILES	= FFT_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

