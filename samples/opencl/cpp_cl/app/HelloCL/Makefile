DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= HelloCL
EXE_TARGET_INSTALL   	= HelloCL

####
#
#  C/CPP files
#
####

FILES 	= HelloCL 
CLFILES = HelloCL_Kernels.cl 

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

