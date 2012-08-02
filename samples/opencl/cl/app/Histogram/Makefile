DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= Histogram
EXE_TARGET_INSTALL   	= Histogram

####
#
#  C/CPP files
#
####

FILES 	= Histogram
CLFILES	= Histogram_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

