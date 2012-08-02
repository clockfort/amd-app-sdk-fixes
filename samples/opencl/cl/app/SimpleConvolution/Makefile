DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= SimpleConvolution
EXE_TARGET_INSTALL   	= SimpleConvolution

####
#
#  C/CPP files
#
####

FILES 	= SimpleConvolution
CLFILES	= SimpleConvolution_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

