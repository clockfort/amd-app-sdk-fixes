DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= HDRToneMapping
EXE_TARGET_INSTALL   	= HDRToneMapping
####
#
#  C/CPP files
#
####

FILES 	= HDRToneMapping
CLFILES = HDRToneMapping_Kernels.cl 
IMAGES = input.hdr

LLIBS  	+= SDKUtil


include $(DEPTH)/make/openclsdkrules.mk 

