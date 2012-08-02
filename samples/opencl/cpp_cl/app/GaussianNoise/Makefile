DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= GaussianNoise
EXE_TARGET_INSTALL   	= GaussianNoise
####
#
#  C/CPP files
#
####

FILES 	= GaussianNoise
CLFILES = GaussianNoise_Kernels.cl 
IMAGES = GaussianNoise_Input.bmp

LLIBS  	+= SDKUtil


include $(DEPTH)/make/openclsdkrules.mk 

