DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= FastWalshTransform
EXE_TARGET_INSTALL   	= FastWalshTransform

####
#
#  C/CPP files
#
####

FILES 	= FastWalshTransform
CLFILES	= FastWalshTransform_Kernels.cl

LLIBS  	+= SDKUtil


include $(DEPTH)/make/openclsdkrules.mk 

