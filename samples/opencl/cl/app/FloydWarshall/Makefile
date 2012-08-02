DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= FloydWarshall
EXE_TARGET_INSTALL   	= FloydWarshall

####
#
#  C/CPP files
#
####

FILES 	= FloydWarshall
CLFILES	= FloydWarshall_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

