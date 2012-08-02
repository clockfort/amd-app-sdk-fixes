DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= DCT
EXE_TARGET_INSTALL   	= DCT

####
#
#  C/CPP files
#
####

FILES 	= DCT 
CLFILES	= DCT_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

