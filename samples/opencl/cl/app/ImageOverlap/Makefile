DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
INSTALL_TO_PUBLIC       = 1
EXE_TARGET 		= ImageOverlap
EXE_TARGET_INSTALL   	= ImageOverlap

####
#
#  C/CPP files
#
####

FILES 		= ImageOverlap 
CLFILES		= ImageOverlap_Kernels.cl
IMAGES		= ImageOverlap_map.bmp ImageOverlap_verify_map.bmp

LLIBS  		+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

